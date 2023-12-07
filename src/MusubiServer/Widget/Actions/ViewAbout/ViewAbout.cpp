#include "ViewAbout.hpp"

namespace Widget {
ViewAbout::ViewAbout(QObject *parent)
    : QAction(QIcon(":/Widget/Widget/Actions/ViewAbout/ViewAbout.png"),
              tr("View Musubi About"), parent) {
  window = new Window::About("Test", nullptr);

  connect(this, &QAction::triggered, this, &ViewAbout::onTriggered);
}

ViewAbout::~ViewAbout() { window->deleteLater(); }

void ViewAbout::onTriggered() { window->show(); }
} // namespace Widget