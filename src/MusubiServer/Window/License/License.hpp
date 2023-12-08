#ifndef LICENSE_HPP
#define LICENSE_HPP
#include "ui_License.h"
#include <QDialog>

namespace Window {
class License : public QDialog {
  Q_OBJECT;

public:
  License(const QString &user, QWidget *parent = nullptr);

  void setRegisteredUser(const QString &user);

private:
  Ui::License *ui;
};
} // namespace Window
#endif