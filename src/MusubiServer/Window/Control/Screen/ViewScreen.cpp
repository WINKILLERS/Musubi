#include "ViewScreen.h"
#include "AApch.h"
#include "Network/ScreenChannel.h"
#include "qevent.h"
#include "qobject.h"
#include "qpainter.h"

Window::Control::ScreenDisplayer::ScreenDisplayer(QWidget *parent)
    : QWidget(parent) {
  setMouseTracking(true);
}

Window::Control::ScreenDisplayer::~ScreenDisplayer() {}

void Window::Control::ScreenDisplayer::updateScreen(
    QRect rect_, const std::string &buffer_) {
  rect = rect_;
  buffer = buffer_;

  repaint();
}

void Window::Control::ScreenDisplayer::paintEvent(QPaintEvent *ev) {
  if (buffer.empty()) {
    return;
  }

  QPainter painter(this);

  QImage frame((uint8_t *)buffer.data(), rect.width(), rect.height(),
               QImage::Format_RGB16);
  painter.drawPixmap(rect.x(), rect.y(), rect.width(), rect.height(),
                     QPixmap::fromImage(frame));
}

void Window::Control::ScreenDisplayer::mouseMoveEvent(QMouseEvent *ev) {
  auto pos = ev->position();
  emit mouseMoved((double)pos.x() / width(), (double)pos.y() / height());
}

void Window::Control::ScreenDisplayer::mousePressEvent(QMouseEvent *ev) {
  auto pos = ev->position();
  emit mousePressed((double)pos.x() / width(), (double)pos.y() / height(),
                    ev->button());
}

void Window::Control::ScreenDisplayer::mouseReleaseEvent(QMouseEvent *ev) {
  auto pos = ev->position();
  emit mouseReleased((double)pos.x() / width(), (double)pos.y() / height(),
                     ev->button());
}

void Window::Control::ScreenDisplayer::mouseDoubleClickEvent(QMouseEvent *ev) {
  auto pos = ev->position();
  emit mouseDoubleClicked((double)pos.x() / width(), (double)pos.y() / height(),
                          ev->button());
}

void Window::Control::ScreenDisplayer::wheelEvent(QWheelEvent *ev) {
  emit wheelMoved(ev->angleDelta().y());
}

void Window::Control::ScreenDisplayer::keyPressEvent(QKeyEvent *ev) {
  emit keyPressed(ev->nativeVirtualKey());
}

void Window::Control::ScreenDisplayer::keyReleaseEvent(QKeyEvent *ev) {
  emit keyReleased(ev->nativeVirtualKey());
}

Window::Control::ViewScreen::ViewScreen(Network::ScreenChannel *channel,
                                        QWidget *parent)
    : QDialog(parent), channel(channel), ui(new Ui::ViewScreen()) {
  spdlog::debug("initializing remote screen");

  ui->setupUi(this);

  // Enable maximize button
  setWindowFlags(windowFlags() | Qt::WindowMaximizeButtonHint);

  displayer = new ScreenDisplayer(this);

  ui->screen_layout->addWidget(displayer);

  connect(displayer, &ScreenDisplayer::mouseMoved, this,
          &Window::Control::ViewScreen::onMouseMovedRefresh);
  connect(displayer, &ScreenDisplayer::mousePressed, this,
          &Window::Control::ViewScreen::onMousePressedRefresh);
  connect(displayer, &ScreenDisplayer::mouseReleased, this,
          &Window::Control::ViewScreen::onMouseReleasedRefresh);
  connect(displayer, &ScreenDisplayer::mouseDoubleClicked, this,
          &Window::Control::ViewScreen::onMouseDoubleClickRefresh);
  connect(displayer, &ScreenDisplayer::keyPressed, this,
          &Window::Control::ViewScreen::onKeyPressedRefresh);
  connect(displayer, &ScreenDisplayer::keyReleased, this,
          &Window::Control::ViewScreen::onKeyReleasedRefresh);
  connect(displayer, &ScreenDisplayer::wheelMoved, this,
          &Window::Control::ViewScreen::onWheelMovedRefresh);

  connect(ui->detach_window, &QPushButton::clicked, this,
          &Window::Control::ViewScreen::detachDisplayer);
  connect(ui->level_spinbox, &QSpinBox::valueChanged, this,
          &Window::Control::ViewScreen::changeConfiguration);

  connect(channel, &Network::ScreenChannel::rectChanged, displayer,
          &Window::Control::ScreenDisplayer::updateScreen);

  connect(channel, &QObject::destroyed, this, [this]() { killTimer(timer); });

  timer = startTimer(20);
}

Window::Control::ViewScreen::~ViewScreen() {
  spdlog::debug("shutting down remote screen");

  displayer->deleteLater();
  delete ui;
}

void Window::Control::ViewScreen::refresh() {
  bool send_input = ui->enable_input->isChecked();
  channel->setInputEnabled(send_input);

  if (mouse_position.x_percentage != 0 && mouse_position.y_percentage != 0) {
    spdlog::trace("queued cursor move {}, {}", mouse_position.x_percentage,
                  mouse_position.y_percentage);

    mouse_inputs.push_back(mouse_position);
    mouse_position.x_percentage = 0;
    mouse_position.y_percentage = 0;
  }

  if (mouse_inputs.empty() == false) {
    spdlog::trace("queued mouse inputs in size: {}", mouse_inputs.size());

    channel->sendMouse(mouse_inputs);
    mouse_inputs.clear();
  }

  if (keyboard_inputs.empty() == false) {
    spdlog::trace("queued keyboard inputs in size: {}", keyboard_inputs.size());

    channel->sendKeyboard(keyboard_inputs);
    keyboard_inputs.clear();
  }
}

void Window::Control::ViewScreen::timerEvent(QTimerEvent *ev) { refresh(); }

void Window::Control::ViewScreen::onMouseMovedRefresh(double x_percentage,
                                                      double y_percentage) {
  mouse_position.x_percentage = x_percentage;
  mouse_position.y_percentage = y_percentage;
  mouse_position.action = Packet::RequestSetMouse::MouseAction::Move;
}

void Window::Control::ViewScreen::onMousePressedRefresh(double x_percentage,
                                                        double y_percentage,
                                                        uint8_t button) {
  Packet::RequestSetMouse::MouseAction action;

  switch (button) {
  case Qt::LeftButton:
    action = Packet::RequestSetMouse::MouseAction::LeftPressed;
    break;
  case Qt::RightButton:
    action = Packet::RequestSetMouse::MouseAction::RightPressed;
    break;
  case Qt::MiddleButton:
    action = Packet::RequestSetMouse::MouseAction::MiddlePressed;
    break;
  default:
    return;
    break;
  }

  spdlog::trace("mouse action detected at {}, {}, type: {}", x_percentage,
                y_percentage, magic_enum::enum_name(action));

  mouse_inputs.emplace_back(action, x_percentage, y_percentage);
}

void Window::Control::ViewScreen::onMouseReleasedRefresh(double x_percentage,
                                                         double y_percentage,
                                                         uint8_t button) {
  Packet::RequestSetMouse::MouseAction action;

  switch (button) {
  case Qt::LeftButton:
    action = Packet::RequestSetMouse::MouseAction::LeftReleased;
    break;
  case Qt::RightButton:
    action = Packet::RequestSetMouse::MouseAction::RightReleased;
    break;
  case Qt::MiddleButton:
    action = Packet::RequestSetMouse::MouseAction::MiddleReleased;
    break;
  default:
    return;
    break;
  }

  mouse_inputs.emplace_back(action, x_percentage, y_percentage);
}

void Window::Control::ViewScreen::onMouseDoubleClickRefresh(double x_percentage,
                                                            double y_percentage,
                                                            uint8_t button) {
  Packet::RequestSetMouse::MouseAction action;

  switch (button) {
  case Qt::LeftButton:
    action = Packet::RequestSetMouse::MouseAction::LeftDoubleClick;
    break;
  case Qt::RightButton:
    action = Packet::RequestSetMouse::MouseAction::RightDoubleClick;
    break;
  default:
    return;
    break;
  }

  mouse_inputs.emplace_back(action, x_percentage, y_percentage);
}

void Window::Control::ViewScreen::onKeyPressedRefresh(int virtual_code) {
  if (virtual_code == VK_ESCAPE && displayer->isFullScreen()) {
    return;
  }

  keyboard_inputs.emplace_back(
      Packet::RequestSetKeyboard::KeyboardAction::Pressed, virtual_code);
}

void Window::Control::ViewScreen::onKeyReleasedRefresh(int virtual_code) {
  if (virtual_code == VK_ESCAPE && displayer->isFullScreen()) {
    displayer->showNormal();
    return;
  }

  keyboard_inputs.emplace_back(
      Packet::RequestSetKeyboard::KeyboardAction::Released, virtual_code);
}

void Window::Control::ViewScreen::onWheelMovedRefresh(int delta) {
  mouse_inputs.emplace_back(Packet::RequestSetMouse::MouseAction::WheelMove, 0,
                            0, delta);
}

void Window::Control::ViewScreen::detachDisplayer(bool) {
  displayer->setWindowFlag(Qt::Window);
  displayer->showFullScreen();
}

void Window::Control::ViewScreen::changeConfiguration() {
  auto compression_level = ui->level_spinbox->value();
  spdlog::debug("compression level set to {}", compression_level);

  channel->refresh(compression_level);
}