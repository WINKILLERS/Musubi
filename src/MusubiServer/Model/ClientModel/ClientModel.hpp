#ifndef CLIENT_MODEL_HPP
#define CLIENT_MODEL_HPP
#include <QAbstractTableModel>

namespace Model {
class ClientModel : public QAbstractTableModel {
public:
  explicit ClientModel(QObject *parent = nullptr);

private:
};
} // namespace Model
#endif