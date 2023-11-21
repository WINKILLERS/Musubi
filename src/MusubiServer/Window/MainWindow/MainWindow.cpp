#include "MainWindow.hpp"

namespace Window {
MainWindow::MainWindow() : ui(new Ui::MainWindow()), QMainWindow(nullptr) {
  ui->setupUi(this);

  resize(1280, 720);
  setWindowTitle(tr("Musubi Server"));

  // Initialize client table widget
  client_table = new Widget::ClientTable(ui->client_table);
  ui->client_table_layout->addWidget(client_table);

  auto handler = new Network::Handler(Network::Handler::default_port, this);
  handler->run();
}

MainWindow::~MainWindow() { delete ui; }
} // namespace Window