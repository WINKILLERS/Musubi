#include "License.hpp"

namespace Window {
License::License(const QString &user, QWidget *parent)
    : QDialog(parent), ui(new Ui::License()) {
  ui->setupUi(this);

  setRegisteredUser(user);
}

void Window::License::setRegisteredUser(const QString &user) {
  auto text = ui->register_message->document();
  text->setHtml(text->toHtml().arg(user));
}
} // namespace Window