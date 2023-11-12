#include "MainWindow.hpp"

namespace Window {
MainWindow::MainWindow() : ui(new Ui::MainWindow()), QMainWindow(nullptr) {
  ui->setupUi(this);

  // Initialize client table widget
  client_table = new Widget::ClientTable(ui->client_table);
  ui->client_table_layout->addWidget(client_table);
}

MainWindow::~MainWindow() { delete ui; }
} // namespace Window