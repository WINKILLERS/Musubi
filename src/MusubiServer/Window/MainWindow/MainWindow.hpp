#ifndef MAIN_WINDOW_HPP
#define MAIN_WINDOW_HPP
#include "Network/Handler.hpp"
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
  Network::Handler *handler;
};
} // namespace Window
#endif