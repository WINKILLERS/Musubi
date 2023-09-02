#pragma once
#include "AbstractPacket.h"

namespace Packet {
class Handshake : public AbstractPacket {
public:
  enum class Role : uint8_t {
    invalid,
    // Main controller, can not request by server, sends when new client
    // connects
    controller,
    // Request establish file channel
    file,
    // Request establish remote screen channel
    remote_screen,
    // Request establish remote hidden screen channel
    remote_hidden_screen,
    // Request establish keyboard monitor channel
    keyboard_monitor,
    // Request establish heartbeat channel
    heartbeat,
    // Request establish program channel
    program,
    // Request establish input channel
    input,
  };

  Role role;
  // Client's hwid
  std::string message;

  Handshake() = default;
  Handshake(const std::string &message, const Role &role = Role::controller)
      : message(message), role(role) {}

  std::string buildJson() const override final;
  void parseJson(const std::string &buffer) override final;

  enum { PacketType = (uint32_t)Type::handshake };
  inline Type getType() const override final { return (Type)PacketType; }

  inline bool operator==(const Handshake &other) const {
    return role == other.role && message == other.message;
  }
};

class Disconnect : public AbstractPacket {
public:
  enum Action : uint8_t {
    disconnect = 0,
    remove = 1,
  };

  Action action;

  Disconnect(const Action &action = Action::disconnect) : action(action) {}

  std::string buildJson() const override final;
  void parseJson(const std::string &buffer) override final;

  inline Type getType() const override final {
    return Type::request_disconnect;
  }

  inline bool operator==(const Disconnect &other) const {
    return action == other.action;
  }
};

class RequestReinitialize : public AbstractPacket {
public:
  RequestReinitialize() = default;

  std::string buildJson() const override final;
  void parseJson(const std::string &buffer) override final;

  inline Type getType() const override final {
    return Type::request_reinitialize;
  }
};

class ResponseReinitialize : public AbstractPacket {
public:
  uint32_t error_code;

  ResponseReinitialize() = default;
  ResponseReinitialize(const uint32_t &error_code) : error_code(error_code) {}

  std::string buildJson() const override final;
  void parseJson(const std::string &buffer) override final;

  inline Type getType() const override final {
    return Type::response_reinitialize;
  }
};
} // namespace Packet