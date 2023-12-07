#ifndef ABOUT_HPP
#define ABOUT_HPP
#include "ui_About.h"
#include <QDialog>

namespace Window {
class About : public QDialog {
  Q_OBJECT;

public:
  About(const QString &user, QWidget *parent = nullptr);

  void setRegisteredUser(const QString &user);

private:
  Ui::About *ui;
};
} // namespace Window
#endif