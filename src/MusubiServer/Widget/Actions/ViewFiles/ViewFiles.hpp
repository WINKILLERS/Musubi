#ifndef VIEW_FILES_ACTION_HPP
#define VIEW_FILES_ACTION_HPP
#include "Model/ClientModel/ClientModel.hpp"
#include <QAction>
#include <QItemSelectionModel>

namespace Widget {
class ViewFiles : public QAction {
  Q_OBJECT;

public:
  ViewFiles(QItemSelectionModel *selection_, Model::ClientModel *model_,
            QObject *parent = nullptr);

public slots:
  void onTriggered();

private:
  QItemSelectionModel *selection;
  Model::ClientModel *model;
};
} // namespace Widget
#endif