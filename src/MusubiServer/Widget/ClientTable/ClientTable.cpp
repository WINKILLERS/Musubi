#include "ClientTable.hpp"
#include <QHeaderView>

namespace Widget {
ClientTable::ClientTable(QWidget *parent) : QTableView(parent) {
  setEditTriggers(QAbstractItemView::NoEditTriggers);
  setSelectionMode(QAbstractItemView::SingleSelection);
  setSelectionBehavior(QAbstractItemView::SelectRows);
  horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
}
} // namespace Widget