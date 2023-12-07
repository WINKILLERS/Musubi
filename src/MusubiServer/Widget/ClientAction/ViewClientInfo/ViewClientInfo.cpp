#include "ViewClientInfo.hpp"
#include "Network/Session.hpp"
#include "Window/ClientInformation/ClientInformation.hpp"

namespace Widget {
ViewClientInfo::ViewClientInfo(QItemSelectionModel *selection_,
                               Model::ClientModel *model_, QObject *parent)
    : QAction(
          QIcon(
              ":/Widget/Widget/ClientAction/ViewClientInfo/ViewClientInfo.png"),
          tr("View Client Info"), parent),
      selection(selection_), model(model_) {
  connect(this, &QAction::triggered, this, &ViewClientInfo::onTriggered);
}

void ViewClientInfo::onTriggered() {
  auto indexes = selection->selectedIndexes();
  auto row = indexes[0].row();
  auto client = model->getClient(row);
  auto window = new Window::ClientInformation(client, nullptr);
  client->addWindow(window);
  window->show();
}
} // namespace Widget