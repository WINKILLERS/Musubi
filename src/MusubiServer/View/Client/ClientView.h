#pragma once
#include "QTableView"

namespace View {
namespace Client {
class ClientView : public QTableView {
  Q_OBJECT

public:
  explicit ClientView(QWidget *parent = nullptr);
  ~ClientView();

private:
};
} // namespace Client
} // namespace View