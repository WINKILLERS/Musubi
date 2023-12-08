#include "ProcessManager.hpp"
#include "NewProcess/NewProcess.hpp"
#include <QLineEdit>
#include <QMessageBox>

namespace Window {
ProcessManager::ProcessManager(Network::Session *session_, QWidget *parent)
    : ui(new Ui::ProcessManager()), QDialog(parent), session(session_) {
  ui->setupUi(this);

  refreshCompleted();

  connect(session, &Network::Session::recvResponseGetProcesses, this,
          &ProcessManager::refreshCompleted);
  connect(session, &Network::Session::recvResponseTerminateProcess, this,
          &ProcessManager::onResponseTerminateProcess);
  connect(session, &Network::Session::recvResponseStartProcess, this,
          &ProcessManager::onResponseStartProcess);

  connect(this, &ProcessManager::terminatingProcess, session,
          &Network::Session::terminateProcess);
  connect(this, &ProcessManager::startingNewProcess, session,
          &Network::Session::startProcess);

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

void ProcessManager::start() {
  auto input = new NewProcess("path\\to\\program.exe", this);
  auto action = input->exec();
  if (action == QDialog::Accepted) {
    Bridge::RequestStartProcess request(input->getPath(),
                                        Bridge::ShowType::show);

    emit startingNewProcess(request);
  }
  input->deleteLater();
}

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

void ProcessManager::onResponseTerminateProcess(
    Bridge ::HeaderPtr header,
    std ::shared_ptr<Bridge ::ResponseTerminateProcess> packet) {
  QMetaObject::invokeMethod(session, &Network::Session::refreshProcesses);

  QString text;
  const auto &statuses = packet->statuses;

  for (const auto &status : statuses) {
    text.append(tr("Terminated process: %1, status: %2\n")
                    .arg(status.pid)
                    .arg(status.success ? "successful" : "failed"));
  }

  QMessageBox::information(this, tr("Terminate Completed"), text);
}

void ProcessManager::onResponseStartProcess(
    Bridge ::HeaderPtr header,
    std ::shared_ptr<Bridge ::ResponseStartProcess> packet) {
  QMetaObject::invokeMethod(session, &Network::Session::refreshProcesses);

  const auto &status = packet->status;
  auto text = tr("Started process: %1, status: %2\n")
                  .arg(QString::fromStdString(status.path))
                  .arg(status.error_code == 0 ? "successful" : "failed");

  QMessageBox::information(this, tr("Start Completed"), text);
}
} // namespace Window