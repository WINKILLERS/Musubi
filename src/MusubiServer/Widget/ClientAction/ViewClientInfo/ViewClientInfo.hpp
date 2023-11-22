#ifndef VIEW_CLIENT_INFO_ACTION_HPP
#define VIEW_CLIENT_INFO_ACTION_HPP
#include "Model/ClientModel/ClientModel.hpp"
#include <QAction>
#include <QItemSelectionModel>

namespace Widget {
class ViewClientInfo : public QAction {
  Q_OBJECT;

public:
  explicit ViewClientInfo(QItemSelectionModel *selection_,
                          Model::ClientModel *model_,
                          QObject *parent = nullptr);

public slots:
  void onTriggered();

private:
  QItemSelectionModel *selection;
  Model::ClientModel *model;
};
} // namespace Widget
#endif