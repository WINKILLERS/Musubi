#pragma once
#include "AbstractPacket.h"

namespace Packet {
class RequestTaskAutoStart : public AbstractPacket {
public:
  enum class TaskType {
    com_default_system,
    com_default_current_user,
  };

  TaskType type;

  RequestTaskAutoStart() = default;
  RequestTaskAutoStart(const TaskType &type) : type(type) {}

  std::string buildJson() const override final;
  void parseJson(const std::string &buffer) override final;

  enum { PacketType = (uint32_t)Type::request_task_autostart };
  inline Type getType() const override final { return (Type)PacketType; }

  inline bool operator==(const RequestTaskAutoStart &other) const {
    return type == other.type;
  }
};

class ResponseTaskAutoStart : public AbstractPacket {
public:
  uint32_t error_code;

  ResponseTaskAutoStart() = default;
  ResponseTaskAutoStart(const uint32_t &error_code) : error_code(error_code) {}

  std::string buildJson() const override final;
  void parseJson(const std::string &buffer) override final;

  enum { PacketType = (uint32_t)Type::response_task_autostart };
  inline Type getType() const override final { return (Type)PacketType; }

  inline bool operator==(const ResponseTaskAutoStart &other) const {
    return error_code == other.error_code;
  }
};
} // namespace Packet