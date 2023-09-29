#pragma once
#include "License.h"
#include "Model/Client/ClientModel.h"
#include "Network/AbstractProtocol.h"
#include "Network/Tcp.h"
#include "Setting.h"
#include "View/Client/ClientView.h"
#include "qlabel.h"
#include "qmainwindow.h"
#include "ui_MusubiServer.h"

namespace Window {
namespace MainWindow {
class MusubiServer : public QMainWindow {
  Q_OBJECT

public:
  MusubiServer(Setting *setting, QWidget *parent = nullptr);
  ~MusubiServer();

private slots:
  void actShowSetting();
  void actShowLicense();
  void actShowViewFile();
  void actShowProgram();
  void actShowNormalScreen();

  void updateCount();

private:
  Network::Controller *getSelection();
  bool openSubchannel(Packet::Handshake::Role role);

  Ui::MusubiServerClass *ui = nullptr;

  Network::TcpHandler *handler = nullptr;
  QThread *handler_pool = nullptr;

  View::Client::ClientView *client_view = nullptr;
  Model::Client::ClientModel *client_model = nullptr;
  QItemSelectionModel *selection = nullptr;

  QLabel *label_address = nullptr;
  QLabel *label_count = nullptr;

  Setting *setting = nullptr;
  License *license = nullptr;
};
} // namespace MainWindow
} // namespace Window