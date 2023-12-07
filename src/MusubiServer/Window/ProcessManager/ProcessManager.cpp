#include "ProcessManager.hpp"
#include <QMessageBox>

namespace Window {
ProcessManager::ProcessManager(Network::Session *session_, QWidget *parent)
    : ui(new Ui::ProcessManager()), QDialog(parent), session(session_) {
  ui->setupUi(this);

  refreshCompleted();

  connect(session, &Network::Session::recvResponseGetProcesses, this,
          &ProcessManager::refreshCompleted);
  connect(session, &Network::Session::recvResponseTerminateProcess, this,
          &ProcessManager::terminateCompleted);

  connect(this, &ProcessManager::terminatingProcess, session,
          &Network::Session::terminateProcess);

  connect(ui->refresh, &QPushButton::clicked, session,
          &Network::Session::refreshProcesses);
  connect(ui->start, &QPushButton::clicked, this, &ProcessManager::start);
  connect(ui->terminate, &QPushButton::clicked, this,
          &ProcessManager::terminate);
}

ProcessManager::~ProcessManager() { delete ui; }

QString ProcessManager::getColumnText(const ColumnItem section) {
  QString text;

  switch (section) {
  case ColumnItem::name:
    text = tr("Process Name");
    break;
  case ColumnItem::pid:
    text = tr("Process ID");
    break;
  case ColumnItem::ppid:
    text = tr("Parent PID");
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

QString ProcessManager::getData(const Bridge::Process &process,
                                const ColumnItem section) {
  QString text;

  switch (section) {
  case ColumnItem::name:
    text = QString::fromStdString(process.name);
    break;
  case ColumnItem::pid:
    text = QString::number(process.pid);
    break;
  case ColumnItem::ppid:
    text = QString::number(process.ppid);
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

void ProcessManager::refreshCompleted() {
  const auto &processes = session->getProcesses();

  ui->table->clear();
  ui->table->setRowCount(processes.size());
  ui->table->setColumnCount(ColumnItem::max_item);
  ui->table->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);

  for (const auto &column : magic_enum::enum_values<ColumnItem>()) {
    ui->table->setHorizontalHeaderItem(
        column, new QTableWidgetItem(getColumnText(column)));
  }

  auto index = 0;
  for (const auto &process : processes) {
    for (const auto &column : magic_enum::enum_values<ColumnItem>()) {
      ui->table->setItem(index, column,
                         new QTableWidgetItem(getData(process, column)));
    }
    index++;
  }
}

void ProcessManager::start() {}

void ProcessManager::terminate() {
  auto selection = ui->table->selectedItems();
  Bridge::RequestTerminateProcess request;

  for (const auto &item : selection) {
    if (item->column() == ColumnItem::pid) {
      auto pid = item->data(Qt::DisplayRole).toULongLong();
      request.addProcess(pid);
    }
  }

  emit terminatingProcess(request);
}

void ProcessManager::terminateCompleted(
    Bridge ::HeaderPtr header,
    std ::shared_ptr<Bridge ::ResponseTerminateProcess> packet) {
  QMetaObject::invokeMethod(session, &Network::Session::refreshProcesses);

  QString text;
  const auto &status = packet->processes;

  for (const auto &process : status) {
    text.append(tr("Terminated process: %1, status: %2\n")
                    .arg(process.pid)
                    .arg(process.success ? "successful" : "failed"));
  }

  QMessageBox::information(this, tr("Terminate Completed"), text);
}
} // namespace Window