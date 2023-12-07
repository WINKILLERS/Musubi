#ifndef CLIENT_INFORMATION_HPP
#define CLIENT_INFORMATION_HPP
#include "Network/Session.hpp"
#include "ui_ClientInformation.h"
#include <QDialog>

namespace Window {
class ClientInformation : public QDialog {
  Q_OBJECT;

public:
  ClientInformation(Network::Session *session_, QWidget *parent = nullptr);
  ~ClientInformation();

private:
  Ui::ClientInformation *ui;
  Network::Session *session;
};
} // namespace Window
#endif