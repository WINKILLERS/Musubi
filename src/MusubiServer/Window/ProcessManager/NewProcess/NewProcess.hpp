#ifndef NEW_PROCESS_HPP
#define NEW_PROCESS_HPP
#include "ui_NewProcess.h"
#include <QDialog>

namespace Window {
class NewProcess : public QDialog {
  Q_OBJECT;

public:
  NewProcess(const std::string &default_path = "", QWidget *parent = nullptr);
  ~NewProcess();

  std::string getPath();

private:
  Ui::NewProcess *ui;
};
} // namespace Window
#endif