#include "Window/MainWinodw/MainWindow.hpp"
#include <QApplication>

int main(int argc, char *argv[]) {
  QApplication app(argc, argv);

  auto main_window = new Window::MainWindow();
  main_window->show();

  return app.exec();
}