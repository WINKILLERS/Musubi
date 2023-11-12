#ifndef HANDSHAKE_HPP
#define HANDSHAKE_HPP
#include "Packet.hpp"
#include <string>

namespace Bridge {
class ClientHandshake : public AbstractPacket {
public:
  IMPLEMENT_AS_PACKET(client_handshake);

  std::string hwid;

  ClientHandshake() = default;
  ClientHandshake(const std::string &hwid_) : hwid(hwid_){};
};

class ServerHandshake : public AbstractPacket {
public:
  IMPLEMENT_AS_PACKET_DEFAULT(server_handshake);

  ServerHandshake() = default;
};
} // namespace Bridge
#endif