#include "FileManager.hpp"
#include "Util/Util.hpp"
#include <QMessageBox>
#include <fmt/chrono.h>
#include <fmt/format.h>

namespace Window {
FileManager::FileManager(Network::Session *session_, QWidget *parent)
    : ui(new Ui::FileManager()), QDialog(parent), session(session_),
      folder_icon(":/Widget/Window/FileManager/folder.png"),
      file_icon(":/Widget/Window/FileManager/file.png") {
  ui->setupUi(this);

  connect(session, &Network::Session::recvResponseGetFiles, this,
          &FileManager::onResponseGetFiles);
  connect(session, &Network::Session::recvResponseRemoveFiles, this,
          &FileManager::onResponseRemoveFiles);

  connect(this, &FileManager::refreshing, session,
          &Network::Session::refreshFiles);
  connect(this, &FileManager::removing, session,
          &Network::Session::removeFiles);

  connect(ui->refresh, &QPushButton::clicked, this, &FileManager::refresh);
  connect(ui->remove, &QPushButton::clicked, this, &FileManager::remove);

  connect(ui->table, &QTableWidget::cellDoubleClicked, this,
          &FileManager::openDirectoryByRow);
  connect(ui->current_directory, &QLineEdit::returnPressed, this,
          &FileManager::openDirectoryByPath);
  connect(ui->previous, &QPushButton::clicked, this, &FileManager::previous);
  connect(ui->next, &QPushButton::clicked, this, &FileManager::next);
  connect(ui->table, &QTableWidget::itemSelectionChanged, this,
          &FileManager::selectionChanged);

  refresh();
}

FileManager::~FileManager() { delete ui; }

QString FileManager::getColumnText(const ColumnItem section) {
  QString text;

  switch (section) {
  case ColumnItem::name:
    text = tr("File Name");
    break;
  case ColumnItem::size:
    text = tr("File Size");
    break;
  case ColumnItem::last_write_time:
    text = tr("Last Write Time");
    break;
  case ColumnItem::max_item:
    break;
  default:
    text = tr("Unknown Column");
    assert(false);
    break;
  }

  return text;
}

QString FileManager::getData(const Bridge::File &file,
                             const ColumnItem section) {
  QString text;

  switch (section) {
  case ColumnItem::name:
    text = QString::fromStdString(file.name);
    break;
  case ColumnItem::size:
    text = QString::number(file.size);
    break;
  case ColumnItem::last_write_time:
    text = getTime(file.last_write_time);
    break;
  case ColumnItem::max_item:
    break;
  default:
    text = tr("Unknown Data");
    assert(false);
    break;
  }

  return text;
}

QString FileManager::getTime(uint64_t timestamp) {
  auto time = std::chrono::time_point<std::chrono::file_clock>(
      std::chrono::milliseconds(timestamp));
  return QString::fromStdString(std::format("{:%F %T}", time));
}

QIcon FileManager::getIcon(const Bridge::File &file) {
  if (file.is_directory) {
    return folder_icon;
  } else {
    return file_icon;
  }
}

std::vector<std::string> FileManager::getSelections() {
  std::vector<std::string> selections;

  for (const auto &item : ui->table->selectedItems()) {
    if (item->column() == ColumnItem::name) {
      selections.emplace_back(
          item->data(Qt::DisplayRole).toString().toStdString());
    }
  }

  return selections;
}

void FileManager::openDirectoryByRow(int row, int column) {
  auto directory_name = ui->table->item(row, ColumnItem::name)
                            ->data(Qt::DisplayRole)
                            .toString()
                            .toStdString();

  // Check if we need to go back root
  if (current_directory == current_directory.root_path() &&
      directory_name == "..") {
    // Open the root
    openRoot();
    return;
  }

  // Check if directory_name is in directories
  auto pos =
      std::find_if(files.cbegin(), files.cend(), [&](const Bridge::File &file) {
        return file.name == directory_name && file.is_directory;
      });

  if (pos == files.cend()) {
    QMessageBox::critical(this, tr("Open Error"),
                          tr("Can not open a a non-exist directory"));
    return;
  }

  auto absolute = current_directory / Util::utf8to8(directory_name);
  absolute = absolute.lexically_normal();
  openAbsolute(absolute);
}

void FileManager::openDirectoryByPath() {
  openAbsolute(ui->current_directory->text().toStdString());
}

void FileManager::openRoot() { openAbsolute(""); }

void FileManager::openAbsolute(const std::filesystem::path &path) {
  previous_directories.push(current_directory);
  ui->previous->setEnabled(true);
  current_directory = path;
  operation = Operation::user_open;
  refresh();
}

void FileManager::refresh() {
  Bridge::RequestGetFiles request;

  ui->current_directory->setText(
      QString::fromStdWString(current_directory.wstring()));
  request.directory = Util::utf8to8(current_directory.u8string());

  ui->table->setEnabled(false);

  emit refreshing(request);
}

void FileManager::previous() {
  // If we have no step back
  if (previous_directories.empty()) {
    return;
  }

  // Save current directory
  next_directories.push(current_directory);
  ui->next->setEnabled(true);

  // Pop previous directory
  current_directory = previous_directories.top();
  previous_directories.pop();

  if (previous_directories.empty()) {
    ui->previous->setEnabled(false);
  }

  // Request update
  operation = Operation::previous;
  refresh();

  return;
}

void FileManager::next() {
  // If we have no step next
  if (next_directories.empty()) {
    return;
  }

  // Save current directory
  previous_directories.push(current_directory);
  ui->previous->setEnabled(true);

  // Pop next directory
  current_directory = next_directories.top();
  next_directories.pop();

  if (next_directories.empty()) {
    ui->next->setEnabled(false);
  }

  // Request update
  operation = Operation::next;
  refresh();

  return;
}

void FileManager::remove() {
  auto selections = getSelections();
  Bridge::RequestRemoveFiles request;

  for (const auto &selection : selections) {
    request.addPath(Util::utf8to8(
        (current_directory / Util::utf8to16(selection)).u8string()));
  }

  emit removing(request);
}

void FileManager::onResponseGetFiles(
    Bridge ::HeaderPtr header,
    std ::shared_ptr<Bridge ::ResponseGetFiles> packet) {
  const auto &error_code = packet->error_code;

  // Check whether the operation is succeed
  if (error_code != 0) {
    QMessageBox::critical(
        this, tr("Open Error"),
        tr("Can not open specific directory, error code: %1").arg(error_code));

    // If the request is failed because next
    if (operation == Operation::next || operation == Operation::user_open) {
      previous();
    }
    // Or by previous or user open
    else if (operation == Operation::previous) {
      next();
    } else {
      openRoot();
    }

    return;
  }

  if (operation == Operation::user_open) {
    // Clear next directories
    next_directories = std::stack<std::filesystem::path>();
    ui->next->setEnabled(false);
  }

  files = packet->files;

  std::sort(files.begin(), files.end(),
            [](const Bridge::File &p1, const Bridge::File &p2) {
              auto lower_p1 = p1.name;
              auto lower_p2 = p2.name;
              auto tolower = [](auto c) { return std::tolower(c); };
              std::transform(lower_p1.begin(), lower_p1.end(), lower_p1.begin(),
                             tolower);
              std::transform(lower_p2.begin(), lower_p2.end(), lower_p2.begin(),
                             tolower);

              if (p1.is_directory == p2.is_directory) {
                return lower_p1 < lower_p2;
              } else {
                return p1.is_directory;
              }
            });
  files.emplace(files.cbegin(), "..", true, 0, 0);

  auto width = ui->table->width() - 30;
  ui->table->clear();
  ui->table->setRowCount(files.size());
  ui->table->setColumnCount(ColumnItem::max_item);
  ui->table->setColumnWidth(ColumnItem::name, width / 2);
  ui->table->setColumnWidth(ColumnItem::size, width / 4);
  ui->table->setColumnWidth(ColumnItem::last_write_time, width / 4);

  for (const auto &column : magic_enum::enum_values<ColumnItem>()) {
    ui->table->setHorizontalHeaderItem(
        column, new QTableWidgetItem(getColumnText(column)));
  }

  auto index = 0;
  for (const auto &file : files) {
    for (const auto &column : magic_enum::enum_values<ColumnItem>()) {
      if (column == ColumnItem::name) {
        ui->table->setItem(
            index, column,
            new QTableWidgetItem(getIcon(file), getData(file, column)));
      } else {
        ui->table->setItem(index, column,
                           new QTableWidgetItem(getData(file, column)));
      }
    }
    index++;
  }

  ui->table->setEnabled(true);
}

void FileManager::onResponseRemoveFiles(
    Bridge ::HeaderPtr header,
    std ::shared_ptr<Bridge ::ResponseRemoveFiles> packet) {
  const auto &statuses = packet->statuses;
  QString text;

  for (const auto &status : statuses) {
    auto path = QString::fromStdString(status.path);
    auto error_code = status.error_code;
    text.append(tr("Removed %1, status: %2\n").arg(path).arg(error_code));
  }

  QMessageBox::information(this, tr("Remove File"), text);

  refresh();
}

void FileManager::selectionChanged() {
  auto has_selection = ui->table->selectedItems().empty() != true;

  ui->download->setEnabled(has_selection);
  ui->open->setEnabled(has_selection);
  ui->rename->setEnabled(has_selection);
  ui->remove->setEnabled(has_selection);
}
} // namespace Window
