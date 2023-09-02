#include "License.h"
#include "AApch.h"

Window::MainWindow::License::License(QWidget *parent)
    : QDialog(parent), ui(new Ui::License()) {
  ui->setupUi(this);
}

Window::MainWindow::License::License(const QString &user, bool show,
                                     QWidget *parent)
    : user(user), is_show(show), QDialog(parent), ui(new Ui::License()) {
  ui->setupUi(this);

  showRegistered(is_show);
  setRegisteredUser(user);
}

Window::MainWindow::License::~License() { delete ui; }

void Window::MainWindow::License::showRegistered(bool show_) {
  is_show = show_;

  if (is_show)
    ui->register_message->show();
  else
    ui->register_message->hide();
}

void Window::MainWindow::License::setRegisteredUser(QString user_) {
  user = user_;

  auto text = ui->register_message->document();
  text->setHtml(text->toHtml().arg(user));
}