#ifndef HEARTBEAT_HPP
#define HEARTBEAT_HPP
#include "Packet.hpp"

namespace Bridge {
class Heartbeat : public AbstractPacket {
public:
  IMPLEMENT_AS_PACKET_DEFAULT(heartbeat);

  Heartbeat() = default;
};
} // namespace Bridge
#endif