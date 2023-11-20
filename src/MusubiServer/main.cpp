#include "Window/Logger/Logger.hpp"
#include "Window/MainWinodw/MainWindow.hpp"
#include "qobject.h"
#include <QApplication>
#include <spdlog/sinks/qt_sinks.h>
#include <spdlog/spdlog.h>

int main(int argc, char *argv[]) {
  QApplication app(argc, argv);

  // Build logger window
  auto logger_window = new Window::Logger(nullptr);
  logger_window->show();
  logger_window->setAttribute(Qt::WA_DeleteOnClose);

  // Build spdlog sink
  auto qt_logger =
      std::make_shared<spdlog::sinks::qt_color_sink_mt>(logger_window, 500);

  // Set as default logger
  spdlog::set_default_logger(std::make_shared<spdlog::logger>(
      "Musubi", spdlog::sinks_init_list{qt_logger}));

  // Auto scroll log
  QObject::connect(logger_window, &QTextEdit::textChanged, [logger_window]() {
    logger_window->moveCursor(QTextCursor::End);
  });

  spdlog::info("server started");

  // Build main window
  auto main_window = new Window::MainWindow();
  main_window->show();
  main_window->setAttribute(Qt::WA_DeleteOnClose);

  // Once main window closed, close logger window
  QObject::connect(main_window, &Window::MainWindow::destroyed, logger_window,
                   &Window::Logger::close);

  return app.exec();
}