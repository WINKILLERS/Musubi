#include <QTableView>

namespace Widget {
class ClientTable : public QTableView {
public:
  explicit ClientTable(QWidget *parent = nullptr);
};
} // namespace Widget