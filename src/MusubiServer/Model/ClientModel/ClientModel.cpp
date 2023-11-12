#include "ClientModel.hpp"
#include "qabstractitemmodel.h"

namespace Model {
ClientModel::ClientModel(QObject *parent) : QAbstractTableModel(parent) {}
} // namespace Model