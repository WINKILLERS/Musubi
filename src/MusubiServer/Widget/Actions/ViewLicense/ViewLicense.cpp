#include "ViewLicense.hpp"

namespace Widget {
ViewLicense::ViewLicense(QObject *parent)
    : QAction(QIcon(":/Widget/Widget/Actions/ViewLicense/ViewLicense.png"),
              tr("View Musubi About"), parent) {
  window = new Window::License("Test", nullptr);

  connect(this, &QAction::triggered, this, &ViewLicense::onTriggered);
}

ViewLicense::~ViewLicense() { window->deleteLater(); }

void ViewLicense::onTriggered() { window->show(); }
} // namespace Widget