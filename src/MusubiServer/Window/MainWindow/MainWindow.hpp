#ifndef MAIN_WINDOW_HPP
#define MAIN_WINDOW_HPP
#include "Model/ClientModel/ClientModel.hpp"
#include "Network/Handler.hpp"
#include "Widget/ClientTable/ClientTable.hpp"
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
  Model::ClientModel *client_model;
  Network::Handler *handler;
};
} // namespace Window
#endif