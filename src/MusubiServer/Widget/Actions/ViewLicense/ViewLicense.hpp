#ifndef VIEW_ABOUT_ACTION_HPP
#define VIEW_ABOUT_ACTION_HPP
#include "Window/License/License.hpp"
#include <QAction>

namespace Widget {
class ViewLicense : public QAction {
  Q_OBJECT;

public:
  explicit ViewLicense(QObject *parent = nullptr);
  ~ViewLicense();

public slots:
  void onTriggered();

private:
  Window::License *window;
};
} // namespace Widget
#endif