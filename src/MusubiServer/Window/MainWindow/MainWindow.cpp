#include "MainWindow.hpp"
#include <QMetaObject>
#include <QThread>
#include <spdlog/spdlog.h>

namespace Window {
MainWindow::MainWindow() : ui(new Ui::MusubiServer()), QMainWindow(nullptr) {
  ui->setupUi(this);

  resize(1280, 720);
  setWindowTitle(tr("Musubi Server"));

  // Initialize client table widget
  client_table = new Widget::ClientTable(this);
  ui->client_table_layout->addWidget(client_table);

  // Initialize tcp handler
  handler_thread = new QThread();
  handler = new Network::Handler(Network::Handler::default_port);
  handler->moveToThread(handler_thread);
  handler_thread->start();
  QMetaObject::invokeMethod(handler, &Network::Handler::run);

  // Set data model
  client_model = new Model::ClientModel(handler, this);
  client_table->setModel(client_model);

  // Get selection model after reset the data model
  selection = client_table->selectionModel();

  // Listen selection change
  connect(selection, &QItemSelectionModel::selectionChanged, this,
          &MainWindow::onTableSelectionChanged);

  // Construct tool bar
  control_bar = new QToolBar(this);
  control_bar->setMovable(false);
  control_bar->setToolButtonStyle(Qt::ToolButtonTextUnderIcon);
  ui->tool_bar_layout->addWidget(control_bar, 0, 0);

  // Fill tools
  action_view_info =
      new Widget::ViewClientInfo(selection, client_model, control_bar);
  action_view_info->setText(tr("View Client Info"));
  control_bar->addAction(action_view_info);
  connect(ui->actionView_Information, &QAction::triggered, action_view_info,
          &Widget::ViewClientInfo::onTriggered);

  action_view_processes =
      new Widget::ViewProcesses(selection, client_model, control_bar);
  action_view_processes->setText(tr("View Processes"));
  control_bar->addAction(action_view_processes);
  connect(ui->actionProcess_Manager, &QAction::triggered, action_view_processes,
          &Widget::ViewProcesses::onTriggered);

  action_view_files =
      new Widget::ViewFiles(selection, client_model, control_bar);
  action_view_files->setText(tr("View Files"));
  control_bar->addAction(action_view_files);
  connect(ui->actionFile_Manager, &QAction::triggered, action_view_files,
          &Widget::ViewFiles::onTriggered);

  action_view_license = new Widget::ViewLicense(this);
  action_view_license->setText(tr("View Musubi About"));
  // control_bar->addAction(action_view_license);
  connect(ui->actionLicense, &QAction::triggered, action_view_license,
          &Widget::ViewLicense::onTriggered);

  control_bar->setEnabled(false);
}

MainWindow::~MainWindow() {
  handler_thread->exit();

  handler->deleteLater();
  handler_thread->deleteLater();
  delete ui;
}

void MainWindow::onTableSelectionChanged(const QItemSelection &selected,
                                         const QItemSelection &deselected) {
  auto has_selection = selection->hasSelection();

  control_bar->setEnabled(has_selection);
  ui->actionView_Information->setEnabled(has_selection);
  ui->actionProcess_Manager->setEnabled(has_selection);
  ui->actionFile_Manager->setEnabled(has_selection);
}
} // namespace Window