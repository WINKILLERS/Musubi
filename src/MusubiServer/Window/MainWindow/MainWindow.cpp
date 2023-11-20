#include "MainWindow.hpp"

namespace Window {
MainWindow::MainWindow() : ui(new Ui::MainWindow()), QMainWindow(nullptr) {
  ui->setupUi(this);

  setWindowTitle(tr("Musubi Server"));

  // Initialize client table widget
  client_table = new Widget::ClientTable(ui->client_table);
  ui->client_table_layout->addWidget(client_table);

  auto handler =
      new Network::TcpHandler(Network::TcpHandler::default_port, this);
  handler->run();
}

MainWindow::~MainWindow() { delete ui; }
} // namespace Window