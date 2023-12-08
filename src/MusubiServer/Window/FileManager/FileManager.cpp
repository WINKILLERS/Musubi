#include "FileManager.hpp"

namespace Window {
FileManager::FileManager(Network::Session *session_, QWidget *parent)
    : ui(new Ui::FileManager()), QDialog(parent), session(session_) {
  ui->setupUi(this);
}

FileManager::~FileManager() { delete ui; }
} // namespace Window