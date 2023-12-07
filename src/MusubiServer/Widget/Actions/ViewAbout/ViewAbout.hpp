#ifndef VIEW_ABOUT_ACTION_HPP
#define VIEW_ABOUT_ACTION_HPP
#include "Window/About/About.hpp"
#include <QAction>

namespace Widget {
class ViewAbout : public QAction {
  Q_OBJECT;

public:
  explicit ViewAbout(QObject *parent = nullptr);
  ~ViewAbout();

public slots:
  void onTriggered();

private:
  Window::About *window;
};
} // namespace Widget
#endif