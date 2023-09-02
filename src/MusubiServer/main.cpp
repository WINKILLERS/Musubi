#include "AApch.h"
#include "Window/MainWindow/MusubiServer.h"
#include "Window/MainWindow/Setting.h"
#include "qapplication.h"
#include "qtextedit.h"
#include "qtranslator.h"
#include "random"
#include "spdlog/sinks/daily_file_sink.h"
#include "spdlog/sinks/qt_sinks.h"
#include "spdlog/sinks/stdout_color_sinks.h"

std::random_device r;
std::default_random_engine e(r());
std::uniform_int_distribution<int> uniform_dist;

int main(int argc, char *argv[]) {
  QApplication a(argc, argv);

  // Build qt logger
  auto text_edit = new QTextEdit(nullptr);
  text_edit->setWindowTitle(QObject::tr("Musubi Logger"));
  text_edit->setReadOnly(true);
  text_edit->resize(800, 600);

  // Initialize file logger
  auto daily_logger =
      std::make_shared<spdlog::sinks::daily_file_sink_mt>("Musubi.log", 0, 0);

  // Initialize qt logger
  auto qt_logger =
      std::make_shared<spdlog::sinks::qt_color_sink_mt>(text_edit, 500);

  // Set as default logger
  spdlog::set_default_logger(std::make_shared<spdlog::logger>(
      "Musubi", spdlog::sinks_init_list{daily_logger, qt_logger}));

  QTranslator translator;
  auto *setting = new Window::MainWindow::Setting();

  // Hide setting window
  setting->hide();

  // Set log level
  spdlog::set_level((spdlog::level::level_enum)setting->getLogLevel());
  spdlog::set_level(spdlog::level::level_enum::debug);

  // Install translation
  auto language_file = setting->getLanguageFile();
  if (language_file.isEmpty() == false) {
    if (translator.load(language_file) == true) {
      spdlog::info("installing translation file");
      a.installTranslator(&translator);
      setting->retranslate();
    } else
      spdlog::error("unable to open translation file");
  }

  // Initialize main window
  auto *w = new Window::MainWindow::MusubiServer(setting);

  // Set setting parent
  setting->setParent(w);
  setting->setWindowFlag(Qt::Window);

  // Show logger
  text_edit->setWindowFlag(Qt::Window);
  text_edit->setWindowFlag(Qt::WindowCloseButtonHint, false);
  text_edit->show();

  // Once main window closed, exit all
  w->setAttribute(Qt::WA_DeleteOnClose);
  QObject::connect(w, &QMainWindow::destroyed, &a, &QApplication::quit);

  // Auto scroll
  QObject::connect(text_edit, &QTextEdit::textChanged,
                   [text_edit]() { text_edit->moveCursor(QTextCursor::End); });

  return a.exec();
}