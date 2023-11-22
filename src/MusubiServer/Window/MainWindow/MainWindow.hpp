#ifndef MAIN_WINDOW_HPP
#define MAIN_WINDOW_HPP
#include "Model/ClientModel/ClientModel.hpp"
#include "Network/Handler.hpp"
#include "Widget/ClientAction/ViewClientInfo/ViewClientInfo.hpp"
#include "Widget/ClientTable/ClientTable.hpp"
#include "ui_MainWinodw.h"
#include <QMainWindow>
#include <QToolBar>

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
  QToolBar *tool_bar;
  Widget::ViewClientInfo *action_view_info;
};
} // namespace Window
#endif