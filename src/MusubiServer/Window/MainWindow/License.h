#pragma once
#include "qdialog.h"
#include "ui_License.h"

namespace Window {
namespace MainWindow {
class License : public QDialog {
  Q_OBJECT

public:
  explicit License(QWidget *parent = nullptr);
  License(const QString &user, QWidget *parent = nullptr);
  ~License();

  void setRegisteredUser(QString user_);

private:
  Ui::License *ui = nullptr;

  QString user;
};
} // namespace MainWindow
} // namespace Window