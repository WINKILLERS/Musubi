#ifndef FILE_MANAGER_HPP
#define FILE_MANAGER_HPP
#include "Network/Session.hpp"
#include "ui_FileManager.h"
#include <QDialog>

namespace Window {
class FileManager : public QDialog {
  Q_OBJECT;

public:
  FileManager(Network::Session *session_, QWidget *parent = nullptr);
  ~FileManager();

private:
  Ui::FileManager *ui;
  Network::Session *session;
};
} // namespace Window
#endif