#ifndef PROCESS_MANAGER_HPP
#define PROCESS_MANAGER_HPP
#include "ui_ProcessManager.h"
#include <QDialog>

namespace Window {
class ProcessManager : public QDialog {
  Q_OBJECT;

public:
private:
  Ui::ProcessManager *ui;
};
} // namespace Window
#endif