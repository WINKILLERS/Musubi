#ifndef PROCESS_MANAGER_HPP
#define PROCESS_MANAGER_HPP
#include "Network/Session.hpp"
#include "ui_ProcessManager.h"
#include <QDialog>

namespace Window {
class ProcessManager : public QDialog {
  Q_OBJECT;

public:
  enum ColumnItem : uint8_t { name = 0, pid, ppid, max_item };

  ProcessManager(Network::Session *session_, QWidget *parent = nullptr);
  ~ProcessManager();

signals:
  void terminatingProcess(const Bridge::RequestTerminateProcess packet);
  void startingNewProcess(const Bridge::RequestStartProcess packet);

private:
  static QString getColumnText(const ColumnItem section);
  static QString getData(const Bridge::Process &process,
                         const ColumnItem section);

  Ui::ProcessManager *ui;
  Network::Session *session;

private slots:
  void refreshCompleted();
  void start();
  void terminate();
  DECLARE_SLOT(ResponseTerminateProcess);
  DECLARE_SLOT(ResponseStartProcess);
};
} // namespace Window
#endif