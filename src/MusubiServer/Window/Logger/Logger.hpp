#ifndef LOGGER_HPP
#define LOGGER_HPP
#include <QTextEdit>

namespace Window {
class Logger : public QTextEdit {
public:
  explicit Logger(QWidget *parent = nullptr);

private:
};
} // namespace Window
#endif