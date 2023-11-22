#ifndef HANDSHAKE_HPP
#define HANDSHAKE_HPP
#include "Packet.hpp"
#include <string>

namespace Bridge {
class ClientInformation : public AbstractPacket {
public:
  IMPLEMENT_AS_PACKET(client_information);

  std::string cpu_model;
  std::string os_name;
  std::string user_name;
  std::string computer_name;

  ClientInformation() = default;
  ClientInformation(const ClientInformation &information)
      : cpu_model(information.cpu_model), os_name(information.os_name),
        user_name(information.user_name),
        computer_name(information.computer_name){};
};

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
  IMPLEMENT_AS_PACKET(server_handshake);

  std::string message;
  bool shutdown;

  ServerHandshake() = default;
  ServerHandshake(const std::string &message_, bool shutdown_)
      : message(message_), shutdown(shutdown_){};
};
} // namespace Bridge
#endif