#include "MainWindow.hpp"
#include "qnamespace.h"

namespace Window {
MainWindow::MainWindow() : ui(new Ui::MainWindow()), QMainWindow(nullptr) {
  ui->setupUi(this);

  resize(1280, 720);
  setWindowTitle(tr("Musubi Server"));

  // Initialize client table widget
  client_table = new Widget::ClientTable(ui->client_table);
  ui->client_table_layout->addWidget(client_table);
  auto selection = client_table->selectionModel();

  handler = new Network::Handler(Network::Handler::default_port, this);
  handler->run();

  client_model = new Model::ClientModel(handler, this);
  client_table->setModel(client_model);

  tool_bar = new QToolBar(this);
  tool_bar->setMovable(false);
  tool_bar->setToolButtonStyle(Qt::ToolButtonTextUnderIcon);
  ui->tool_bar_layout->addWidget(tool_bar);

  action_view_info =
      new Widget::ViewClientInfo(selection, client_model, tool_bar);
  action_view_info->setText(tr("View Client Info"));
  tool_bar->addAction(action_view_info);
}

MainWindow::~MainWindow() { delete ui; }
} // namespace Window