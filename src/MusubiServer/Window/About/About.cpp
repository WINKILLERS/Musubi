#include "About.hpp"

namespace Window {
About::About(const QString &user, QWidget *parent)
    : QDialog(parent), ui(new Ui::About()) {
  ui->setupUi(this);

  setRegisteredUser(user);
}

void Window::About::setRegisteredUser(const QString &user) {
  auto text = ui->register_message->document();
  text->setHtml(text->toHtml().arg(user));
}
} // namespace Window