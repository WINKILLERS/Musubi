#include "Network/TcpHandler.hpp"
#include "Widget/ClientTale/ClientTable.hpp"
#include "ui_MainWinodw.h"
#include <QMainWindow>

namespace Window {
class MainWindow : public QMainWindow {
public:
  MainWindow();
  ~MainWindow();

private:
  Ui::MainWindow *ui;
  Widget::ClientTable *client_table;
  Network::TcpHandler *handler;
};
} // namespace Window