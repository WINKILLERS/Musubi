#ifndef CLIENT_TABLE_HPP
#define CLIENT_TABLE_HPP
#include <QTableView>

namespace Widget {
class ClientTable : public QTableView {
public:
  explicit ClientTable(QWidget *parent = nullptr);

private:
};
} // namespace Widget
#endif