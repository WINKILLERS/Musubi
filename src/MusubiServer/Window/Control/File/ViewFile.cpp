#include "ViewFile.h"
#include "AApch.h"
#include "Network/FileChannel.h"
#include "fstream"
#include "qfiledialog.h"
#include "qmessagebox.h"

namespace Window {
namespace Control {
ViewFile::ViewFile(Network::FileChannel *channel, QWidget *parent)
    : QDialog(parent), channel(channel), ui(new Ui::ViewFile()) {
  spdlog::debug("initializing view file");

  ui->setupUi(this);

  model = new QStandardItemModel(this);

  ui->view_file->setModel(model);
  ui->view_file->setEditTriggers(QAbstractItemView::NoEditTriggers);
  ui->view_file->setSelectionBehavior(QAbstractItemView::SelectRows);
  ui->view_file->setIconSize(QSize(36, 36));

  connect(ui->view_file, &QListView::doubleClicked, this,
          &Window::Control::ViewFile::open);
  connect(ui->refresh, &QPushButton::clicked, this,
          &Window::Control::ViewFile::refresh);
  connect(ui->download, &QPushButton::clicked, this,
          &Window::Control::ViewFile::download);
  connect(ui->remove, &QPushButton::clicked, this,
          &Window::Control::ViewFile::remove);
  connect(ui->upload, &QPushButton::clicked, this,
          &Window::Control::ViewFile::upload);
  connect(ui->execute, &QPushButton::clicked, this,
          &Window::Control::ViewFile::execute);
  connect(ui->forward, &QPushButton::clicked, this,
          &Window::Control::ViewFile::forward);
  connect(ui->back, &QPushButton::clicked, this,
          &Window::Control::ViewFile::back);

  connect(channel, &Network::FileChannel::updateCompleted, this,
          &Window::Control::ViewFile::update);

  // Load resource
  ico_file = QIcon(":/MusubiServer/Window/Control/File/ico/file.png");
  ico_folder = QIcon(":/MusubiServer/Window/Control/File/ico/folder.png");

  if (channel->refresh()) {
    ui->view_file->setEnabled(false);
  }
}

ViewFile::~ViewFile() {
  spdlog::debug("shutting down view file");

  model->deleteLater();

  delete ui;

  channel = nullptr;
}

void ViewFile::update() {
  list = channel->getDirectories();
  auto files = channel->getFiles();

  list.insert(list.end(), files.begin(), files.end());

  model->clear();
  for (auto i = 0; i < list.size(); i++) {
    QStandardItem *item = nullptr;

    if (list[i].is_directory) {
      item =
          new QStandardItem(ico_folder, QString::fromStdString(list[i].name));
    } else {
      item = new QStandardItem(ico_file, QString::fromStdString(list[i].name));
    }

    model->setItem(i, item);
  }

  ui->current_dir->setText(
      tr("Current directory: %1")
          .arg(QString::fromStdString(channel->getCurrentDirectory())));

  ui->view_file->setEnabled(true);
}

void ViewFile::open(const QModelIndex &index) {
  auto select_ret = getSelection();
  if (select_ret.has_value() == false) {
    return;
  }

  auto selection = select_ret.value();

  if (channel->open(selection.name) == false) {
    QMessageBox::critical(this, tr("Open Error"),
                          tr("Unable to open a non-directory file"));
    return;
  }

  ui->view_file->setEnabled(false);
}

void ViewFile::download() {
  auto select_ret = getSelection();
  if (select_ret.has_value() == false) {
    return;
  }

  auto selection = select_ret.value();

  auto download_ret = channel->download(selection.name);

  if (download_ret.has_value() == false) {
    QMessageBox::critical(
        this, tr("Download Error"),
        tr("Trying to download a directory or the file did not exist"));
    return;
  }

  ui->download->setEnabled(false);

  auto task = std::move(download_ret.value());

  QFileDialog dialog(this, tr("Save File"), QString());
  std::wstring save_path;
  dialog.setOptions(QFileDialog::DontUseNativeDialog);
  dialog.setAcceptMode(QFileDialog::AcceptSave);
  dialog.setFileMode(QFileDialog::AnyFile);
  dialog.selectFile(QString::fromStdString(selection.name));
  if (dialog.exec()) {
    save_path = dialog.selectedFiles()[0].toStdWString();
  } else {
    return;
  }

  std::ofstream file(save_path, std::ios::binary | std::ios::trunc);
  if (file.is_open() == false) {
    spdlog::error("can not save file at given location");
    return;
  }

  task.then([this, save_path, f = std::move(file)](
                std::shared_ptr<Packet::ResponseDownloadFile> packet) mutable {
    if (f.is_open() && packet->error_code == 0) {
      f.write(packet->content.data(), packet->content.size());
      f.close();
    }

    ui->download->setEnabled(true);

    QMessageBox::information(
        this, tr("Download Complete"),
        tr("Download complete with status: %1, size: %2 kb")
            .arg(packet->error_code)
            .arg((double)packet->content.size() / 1024));
  });
}

void ViewFile::refresh() {
  if (channel->refresh()) {
    ui->view_file->setEnabled(false);
  }
}

void ViewFile::upload() {
  QFileDialog dialog(this, tr("Upload File"), QString());
  std::wstring local_path;
  dialog.setOptions(QFileDialog::DontUseNativeDialog);
  dialog.setAcceptMode(QFileDialog::AcceptOpen);
  dialog.setFileMode(QFileDialog::AnyFile);
  if (dialog.exec()) {
    local_path = dialog.selectedFiles()[0].toStdWString();
  } else {
    return;
  }

  std::filesystem::path path(local_path);

  auto upload_ret =
      channel->upload(local_path, utf8to8(path.filename().u8string()));

  if (upload_ret.has_value() == false) {
    QMessageBox::critical(this, tr("Upload Error"), tr("Something went wrong"));
    return;
  }

  ui->upload->setEnabled(false);

  auto task = std::move(upload_ret.value());

  task.then([this](std::shared_ptr<Packet::ResponseUploadFile> packet) {
    ui->upload->setEnabled(true);

    if (channel->refresh()) {
      ui->view_file->setEnabled(false);
    }

    QMessageBox::information(
        this, tr("Upload Complete"),
        tr("Upload complete with status: %1").arg(packet->error_code));
  });
}

void ViewFile::remove() {
  auto select_ret = getSelection();
  if (select_ret.has_value() == false) {
    return;
  }

  auto selection = select_ret.value();

  auto remove_ret = channel->remove(selection.name);

  if (remove_ret.has_value() == false) {
    QMessageBox::critical(this, tr("Remove Error"), tr("Something went wrong"));
    return;
  }

  ui->remove->setEnabled(false);

  auto task = std::move(remove_ret.value());

  task.then([this](std::shared_ptr<Packet::ResponseDeleteFile> packet) {
    ui->remove->setEnabled(true);

    if (channel->refresh()) {
      ui->view_file->setEnabled(false);
    }

    QMessageBox::information(
        this, tr("Remove Complete"),
        tr("Remove complete, deleted %1 file(s)").arg(packet->removed_count));
  });
}

void ViewFile::execute() {
  auto select_ret = getSelection();
  if (select_ret.has_value() == false) {
    return;
  }

  auto selection = select_ret.value();

  auto execute_ret = channel->execute(selection.name);

  if (execute_ret.has_value() == false) {
    QMessageBox::critical(this, tr("Execute Error"),
                          tr("Something went wrong"));
    return;
  }

  ui->execute->setEnabled(false);

  auto task = std::move(execute_ret.value());

  task.then([this](std::shared_ptr<Packet::ResponseExecuteFile> packet) {
    ui->execute->setEnabled(true);

    QMessageBox::information(
        this, tr("Execute Complete"),
        tr("Execute complete with status: %1").arg(packet->error_code));
  });
}

void ViewFile::forward() {
  channel->next();
  ui->forward->setEnabled(channel->hasNext());
}

void ViewFile::back() {
  channel->previous();
  ui->back->setEnabled(channel->hasPrevious());
}

std::optional<Packet::File> ViewFile::getSelection() {
  auto indexes = ui->view_file->selectionModel()->selectedIndexes();
  if (indexes.isEmpty()) {
    QMessageBox::critical(this, tr("Invalid input"),
                          tr("No file selected, please select one"));
    return std::nullopt;
  }

  return list[indexes[0].row()];
}
} // namespace Control
} // namespace Window