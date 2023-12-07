#include "ClientInformation.hpp"

namespace Window {
ClientInformation::ClientInformation(Network::Session *session_,
                                     QWidget *parent)
    : ui(new Ui::ClientInformation()), QDialog(parent), session(session_) {
  ui->setupUi(this);

  ui->computer_name->setText(
      QString::fromStdString(session->getComputerName()));
}

ClientInformation::~ClientInformation() { delete ui; }
} // namespace Window