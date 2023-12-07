#include "ViewProcesses.hpp"
#include "Network/Session.hpp"
#include "Window/ProcessManager/ProcessManager.hpp"

namespace Widget {
ViewProcesses::ViewProcesses(QItemSelectionModel *selection_,
                             Model::ClientModel *model_, QObject *parent)
    : QAction(QIcon(":/Widget/Widget/Actions/ViewProcesses/ViewProcesses.png"),
              tr("View Processes"), parent),
      selection(selection_), model(model_) {
  connect(this, &QAction::triggered, this, &ViewProcesses::onTriggered);
}

void ViewProcesses::onTriggered() {
  auto indexes = selection->selectedIndexes();
  auto row = indexes[0].row();
  auto client = model->getClient(row);
  auto window = new Window::ProcessManager(client, nullptr);
  client->addWindow(window);
  window->show();
}
} // namespace Widget