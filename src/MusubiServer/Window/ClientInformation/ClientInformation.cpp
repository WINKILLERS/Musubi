#include "ClientInformation.hpp"

namespace Window {
ClientInformation::ClientInformation(Network::Session *session_,
                                     QWidget *parent)
    : ui(new Ui::ClientInformation()), QDialog(parent), session(session_) {
  ui->setupUi(this);

  ui->table->setRowCount((int)IndexItem::max_item);
  ui->table->setColumnCount(1);

  ui->table->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
  for (const auto &index : magic_enum::enum_values<IndexItem>()) {
    ui->table->setVerticalHeaderItem((int)index,
                                     new QTableWidgetItem(getIndexText(index)));
    ui->table->setItem((int)index, 0, new QTableWidgetItem(getData(index)));
  }

  ui->table->setHorizontalHeaderItem(0, new QTableWidgetItem(tr("Value")));
}

ClientInformation::~ClientInformation() { delete ui; }

QString ClientInformation::getIndexText(const IndexItem section) {
  QString text;

  switch (section) {
  case IndexItem::address:
    text = tr("Address");
    break;
  case IndexItem::computer_name:
    text = tr("Computer Name");
    break;
  case IndexItem::user_name:
    text = tr("User Name");
    break;
  case IndexItem::os_name:
    text = tr("OS Name");
    break;
  case IndexItem::cpu_model:
    text = tr("CPU Model");
    break;
  case IndexItem::max_item:
    break;
  default:
    text = tr("Unknown Index");
    assert(false);
    break;
  }

  return text;
}

QString ClientInformation::getData(const IndexItem section) {
  QString text;

  switch (section) {
  case IndexItem::address:
    text = QString::fromStdString(session->getRemoteAddress());
    break;
  case IndexItem::computer_name:
    text = QString::fromStdString(session->getComputerName());
    break;
  case IndexItem::user_name:
    text = QString::fromStdString(session->getUserName());
    break;
  case IndexItem::os_name:
    text = QString::fromStdString(session->getOsName());
    break;
  case IndexItem::cpu_model:
    text = QString::fromStdString(session->getCpuModel());
    break;
  case IndexItem::max_item:
    break;
  default:
    text = tr("Unknown Data");
    assert(false);
    break;
  }

  return text;
}
} // namespace Window