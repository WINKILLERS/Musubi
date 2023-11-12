#include <QAbstractTableModel>

namespace Model {
class ClientModel : public QAbstractTableModel {
public:
  explicit ClientModel(QObject *parent = nullptr);

private:
};
} // namespace Model