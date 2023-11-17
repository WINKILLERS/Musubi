#ifndef HANDSHAKE_HPP
#define HANDSHAKE_HPP
#include "Packet.hpp"
#include <string>

namespace Bridge {
class ClientHandshake : public AbstractPacket {
public:
  IMPLEMENT_AS_PACKET(client_handshake);

  std::string hwid;
  Role role;

  ClientHandshake() = default;
  ClientHandshake(const std::string &hwid_, const Role role_)
      : hwid(hwid_), role(role_){};
};

class ServerHandshake : public AbstractPacket {
public:
  IMPLEMENT_AS_PACKET_DEFAULT(server_handshake);

  ServerHandshake() = default;
};
} // namespace Bridge
#endif