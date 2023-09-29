#include "ScreenChannel.h"
#include "AApch.h"
#include "AbstractProtocol.h"
#include "InputChannel.h"
#include "Window/Control/Screen/ViewScreen.h"

Network::ScreenChannel::ScreenChannel(AbstractSession *session,
                                      AbstractChannel *parent)
    : AbstractMultiChannel(session, parent) {
  connect(&session->notifier, &IPacketNotify::recvRemoteScreen, this,
          &ScreenChannel::recvRemoteScreen);

  connect(&session->notifier, &IPacketNotify::recvRemoteScreen, this,
          &ScreenChannel::update);

  // Listen channel connection
  connect(this, &AbstractMultiChannel::channelConnected, this,
          &ScreenChannel::onChannelConnected);

  requestOpenChannel(Packet::Handshake::Role::input);
}

Network::ScreenChannel::~ScreenChannel() {}

bool Network::ScreenChannel::setArgs(uint8_t compression_level) {
  return session->sendJsonPacket(
      Packet::Generator<Packet::RequestRemoteScreenSetArgs>(compression_level));
}

bool Network::ScreenChannel::sendMouse(
    const std::vector<Packet::RequestSetMouse::MouseInput> &mouse_inputs) {
  auto input = (InputChannel *)getSubChannel(Packet::Handshake::Role::input);

  if (input != nullptr) {
    return input->sendMouse(mouse_inputs);
  }

  return false;
}

bool Network::ScreenChannel::sendKeyboard(
    const std::vector<Packet::RequestSetKeyboard::KeyBoardInput>
        &keyboard_inputs) {
  auto input = (InputChannel *)getSubChannel(Packet::Handshake::Role::input);

  if (input != nullptr) {
    return input->sendKeyboard(keyboard_inputs);
  }

  return false;
}

void Network::ScreenChannel::setInputEnabled(bool is_enable) {
  auto input = (InputChannel *)getSubChannel(Packet::Handshake::Role::input);

  if (input != nullptr) {
    input->setInputEnabled(is_enable);
  }
}

bool Network::ScreenChannel::sendSync() {
  return session->sendJsonPacket(
      Packet::Generator<Packet::RequestRemoteScreenSync>());
}

bool Network::ScreenChannel::showWindow() noexcept {
  if (window == nullptr) {
    window = new Window::Control::ViewScreen(this, nullptr);
  }

  return AbstractChannel::showWindow();
}

void Network::ScreenChannel::update(
    std::shared_ptr<Packet::Header> header,
    std::shared_ptr<Packet::ResponseRemoteScreen> packet) {
  emit rectChanged(QRect(packet->rect.x, packet->rect.y, packet->rect.width,
                         packet->rect.height),
                   std::move(packet->rect.screen));
}

void Network::ScreenChannel::onChannelConnected(Packet::Handshake::Role role,
                                                AbstractChannel *sub_channel) {
  if (role == Packet::Handshake::Role::input) {
  } else {
    spdlog::error("bug detected, unhandled subchannel");
    assert(false);
  }

  connect(sub_channel, &AbstractChannel::lag, this, &ScreenChannel::lag);
}