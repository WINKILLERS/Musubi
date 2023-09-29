#include "InputChannel.h"
#include "AApch.h"
#include "AbstractProtocol.h"

Network::InputChannel::InputChannel(AbstractSession *session,
                                    AbstractChannel *parent)
    : AbstractChannel(session, parent) {}

Network::InputChannel::~InputChannel() {}

bool Network::InputChannel::sendMouse(
    const std::vector<Packet::RequestSetMouse::MouseInput> &mouse_inputs) {
  if (input_enabled == false) {
    return true;
  }

  return session
      ->sendJsonPacket(Packet::Generator<Packet::RequestSetMouse>(mouse_inputs))
      .has_value();
}

bool Network::InputChannel::sendKeyboard(
    const std::vector<Packet::RequestSetKeyboard::KeyBoardInput>
        &keyboard_inputs) {
  if (input_enabled == false) {
    return true;
  }

  return session
      ->sendJsonPacket(
          Packet::Generator<Packet::RequestSetKeyboard>(keyboard_inputs))
      .has_value();
}