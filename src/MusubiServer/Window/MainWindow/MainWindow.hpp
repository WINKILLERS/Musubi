#ifndef MAIN_WINDOW_HPP
#define MAIN_WINDOW_HPP
#include "Model/ClientModel/ClientModel.hpp"
#include "Network/Handler.hpp"
#include "Widget/Actions/ViewClientInfo/ViewClientInfo.hpp"
#include "Widget/Actions/ViewFiles/ViewFiles.hpp"
#include "Widget/Actions/ViewLicense/ViewLicense.hpp"
#include "Widget/Actions/ViewProcesses/ViewProcesses.hpp"
#include "Widget/ClientTable/ClientTable.hpp"
#include "ui_MainWinodw.h"
#include <QMainWindow>
#include <QToolBar>

namespace Window {
class MainWindow : public QMainWindow {
  Q_OBJECT;

public:
  MainWindow();
  ~MainWindow();

private slots:
  void onTableSelectionChanged(const QItemSelection &selected,
                               const QItemSelection &deselected);

private:
  Ui::MusubiServer *ui;
  Widget::ClientTable *client_table;
  Model::ClientModel *client_model;
  QItemSelectionModel *selection;
  Network::Handler *handler;
  QThread *handler_thread;
  QToolBar *control_bar;
  Widget::ViewClientInfo *action_view_info;
  Widget::ViewLicense *action_view_license;
  Widget::ViewProcesses *action_view_processes;
  Widget::ViewFiles *action_view_files;
};
} // namespace Window
#endif