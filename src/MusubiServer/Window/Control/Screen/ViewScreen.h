#pragma once
#include "Protocols.h"
#include "qdialog.h"
#include "ui_ViewScreen.h"

// Forward declare
namespace Network {
class ScreenChannel;
}

namespace Window {
namespace Control {
class ScreenDisplayer : public QWidget {
  Q_OBJECT

public:
  explicit ScreenDisplayer(QWidget *parent = nullptr);
  ~ScreenDisplayer();

signals:
  void mouseMoved(double x_percentage, double y_percentage);
  void mousePressed(double x_percentage, double y_percentage, uint8_t button);
  void mouseReleased(double x_percentage, double y_percentage, uint8_t button);
  void mouseDoubleClicked(double x_percentage, double y_percentage,
                          uint8_t button);

  void wheelMoved(int delta);

  void keyPressed(int virtual_code);
  void keyReleased(int virtual_code);

public slots:
  void updateScreen(QRect rect_, const std::string &buffer_);

protected:
  void paintEvent(QPaintEvent *ev) override;
  void mouseMoveEvent(QMouseEvent *ev) override;
  void mousePressEvent(QMouseEvent *ev) override;
  void mouseReleaseEvent(QMouseEvent *ev) override;
  void mouseDoubleClickEvent(QMouseEvent *ev) override;

  void wheelEvent(QWheelEvent *ev) override;

  void keyPressEvent(QKeyEvent *ev) override;
  void keyReleaseEvent(QKeyEvent *ev) override;

private:
  QRect rect;
  std::string buffer;
};

class ViewScreen : public QDialog {
  Q_OBJECT

public:
  ViewScreen(Network::ScreenChannel *channel, QWidget *parent = nullptr);
  ~ViewScreen();

public slots:
  void refresh();

protected:
  void timerEvent(QTimerEvent *ev) override;

private:
  Ui::ViewScreen *ui;
  Network::ScreenChannel *channel = nullptr;
  ScreenDisplayer *displayer = nullptr;

  Packet::RequestSetMouse::MouseInput mouse_position{};
  std::vector<Packet::RequestSetMouse::MouseInput> mouse_inputs;
  std::vector<Packet::RequestSetKeyboard::KeyBoardInput> keyboard_inputs;

  int timer = 0;

  void onMouseMovedRefresh(double x_percentage, double y_percentage);
  void onMousePressedRefresh(double x_percentage, double y_percentage,
                             uint8_t button);
  void onMouseReleasedRefresh(double x_percentage, double y_percentage,
                              uint8_t button);
  void onMouseDoubleClickRefresh(double x_percentage, double y_percentage,
                                 uint8_t button);
  void onKeyPressedRefresh(int virtual_code);
  void onKeyReleasedRefresh(int virtual_code);
  void onWheelMovedRefresh(int delta);
  void detachDisplayer(bool);
  void changeConfiguration();
};
} // namespace Control
} // namespace Window