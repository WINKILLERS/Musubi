#include "Logger.hpp"
#include "qthread.h"

namespace Window {
Logger::Logger(QWidget *parent) : QTextEdit(parent) {
  setWindowTitle(tr("Musubi Logger"));
  setReadOnly(true);
  resize(800, 600);

  // Set always on top
  setWindowFlag(Qt::WindowStaysOnTopHint);

  // Disable close button
  setWindowFlag(Qt::WindowCloseButtonHint, false);
}

Logger::~Logger() {}
} // namespace Window