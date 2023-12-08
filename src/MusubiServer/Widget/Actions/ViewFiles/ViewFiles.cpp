#include "ViewFiles.hpp"
#include "Network/Session.hpp"
#include "Window/FileManager/FileManager.hpp"

namespace Widget {
ViewFiles::ViewFiles(QItemSelectionModel *selection_,
                     Model::ClientModel *model_, QObject *parent)
    : QAction(QIcon(":/Widget/Widget/Actions/ViewFiles/ViewFiles.png"),
              tr("View Files"), parent),
      selection(selection_), model(model_) {
  connect(this, &QAction::triggered, this, &ViewFiles::onTriggered);
}

void ViewFiles::onTriggered() {
  auto indexes = selection->selectedIndexes();
  auto row = indexes[0].row();
  auto client = model->getClient(row);
  auto window = new Window::FileManager(client, nullptr);
  client->addWindow(window);
  window->show();
}
} // namespace Widget