#include "NewProcess.hpp"

namespace Window {
NewProcess::NewProcess(const std::string &default_path, QWidget *parent)
    : QDialog(parent), ui(new Ui::NewProcess()) {
  ui->setupUi(this);

  setWindowTitle(tr("Starting New Process"));

  ui->path->setText(QString::fromStdString(default_path));

  connect(ui->cancel, &QPushButton::clicked, this, &QDialog::reject);
  connect(ui->start, &QPushButton::clicked, this, &QDialog::accept);
}

std::string NewProcess::getPath() { return ui->path->text().toStdString(); }

NewProcess::~NewProcess() { delete ui; }
} // namespace Window