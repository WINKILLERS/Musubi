#ifndef CLIENT_INFORMATION_HPP
#define CLIENT_INFORMATION_HPP
#include "Network/Session.hpp"
#include "ui_ClientInformation.h"
#include <QDialog>

namespace Window {
class ClientInformation : public QDialog {
  Q_OBJECT;

public:
  enum IndexItem : uint8_t {
    address = 0,
    computer_name,
    user_name,
    os_name,
    cpu_model,
    anti_virus,
    max_item
  };

  ClientInformation(Network::Session *session_, QWidget *parent = nullptr);
  ~ClientInformation();

private:
  static QString getIndexText(const IndexItem section);
  QString getData(const IndexItem section);
  QString getAntiVirus();

  Ui::ClientInformation *ui;
  Network::Session *session;
};
} // namespace Window
#endif