#include "License.h"
#include "AApch.h"

Window::MainWindow::License::License(QWidget *parent)
    : QDialog(parent), ui(new Ui::License()) {
  ui->setupUi(this);
}

Window::MainWindow::License::License(const QString &user, QWidget *parent)
    : user(user), QDialog(parent), ui(new Ui::License()) {
  ui->setupUi(this);

  setRegisteredUser(user);
}

Window::MainWindow::License::~License() { delete ui; }

void Window::MainWindow::License::setRegisteredUser(QString user_) {
  user = user_;

  auto text = ui->register_message->document();
  text->setHtml(text->toHtml().arg(user));
}