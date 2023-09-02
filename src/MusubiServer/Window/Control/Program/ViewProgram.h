#pragma once
#include "Protocols.h"
#include "Util/Util.h"
#include "qdialog.h"
#include "qitemselectionmodel.h"
#include "qstandarditemmodel.h"
#include "qtableview.h"
#include "ui_ViewProgram.h"

// Forward declare
namespace Network {
class ProgramChannel;
}

namespace Window {
namespace Control {
class ViewProgram : public QDialog {
  Q_OBJECT

public:
  ViewProgram(Network::ProgramChannel *channel, QWidget *parent = nullptr);
  ~ViewProgram();

private slots:
  void update();
  void refresh();
  void uninstall();

  void openPath(const QModelIndex &index);

private:
  Ui::ViewProgram *ui = nullptr;
  Network::ProgramChannel *channel = nullptr;

  std::vector<Packet::Program> list;
  QStandardItemModel *model = nullptr;
  QItemSelectionModel *selection = nullptr;
  QTableView *view = nullptr;
};
} // namespace Control
} // namespace Window