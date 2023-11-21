#include "HeartbeatChannel.hpp"
#include "Heartbeat.hpp"
#include <thread>

using namespace std::chrono_literals;

namespace Network {
void HeartbeatChannel::run() {
  performHandshake(Bridge::Role::heartbeat);

  while (true) {
    auto ret = sendJsonPacket(GENERATE_PACKET(Bridge::Heartbeat));

    if (ret == false) {
      return;
    }

    std::this_thread::sleep_for(3s);
  }
}
} // namespace Network