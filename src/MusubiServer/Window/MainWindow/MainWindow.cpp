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
  tool_bar = new QToolBar(this);
  tool_bar->setMovable(false);
  tool_bar->setToolButtonStyle(Qt::ToolButtonTextUnderIcon);
  ui->tool_bar_layout->addWidget(tool_bar);

  // Fill tools
  action_view_info =
      new Widget::ViewClientInfo(selection, client_model, tool_bar);
  action_view_info->setText(tr("View Client Info"));
  tool_bar->addAction(action_view_info);

  tool_bar->setEnabled(false);
}

MainWindow::~MainWindow() {
  handler_thread->exit();

  handler->deleteLater();
  handler_thread->deleteLater();
  delete ui;
}

void MainWindow::onTableSelectionChanged(const QItemSelection &selected,
                                         const QItemSelection &deselected) {
  tool_bar->setEnabled(selection->hasSelection());
}
} // namespace Window