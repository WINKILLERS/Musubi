#include "Heartbeat.h"
#include "AApch.h"
#include "Util/Util.h"

void Network::Heartbeat::run() noexcept {
  performHandshake(Packet::Handshake::Role::heartbeat);

  while (true) {
    if (sendHeartbeat() == false) {
      return;
    }

    std::this_thread::sleep_for(duration);
  }
}

bool Network::Heartbeat::sendHeartbeat() noexcept {
  return sendJsonPacket(
      Packet::Generator<Packet::ResponseHeartbeat>(getFocusWindow()));
}

Network::Heartbeat::~Heartbeat() {}