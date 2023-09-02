#pragma once
#include "AbstractPacket.h"

namespace Packet {
class RequestTerminateProcess : public AbstractPacket {
public:
  uint64_t pid;

  RequestTerminateProcess() = default;
  RequestTerminateProcess(const uint64_t &pid) : pid(pid) {}

  std::string buildJson() const override final;
  void parseJson(const std::string &buffer) override final;

  enum { PacketType = (uint32_t)Type::request_terminate_process };
  inline Type getType() const override final { return (Type)PacketType; }

  inline bool operator==(const RequestTerminateProcess &other) const {
    return pid == other.pid;
  }
};

class ResponseTerminateProcess : public AbstractPacket {
public:
  uint32_t error_code;

  ResponseTerminateProcess() = default;
  ResponseTerminateProcess(const uint32_t &error_code)
      : error_code(error_code) {}

  std::string buildJson() const override final;
  void parseJson(const std::string &buffer) override final;

  enum { PacketType = (uint32_t)Type::response_terminate_process };
  inline Type getType() const override final { return (Type)PacketType; }

  inline bool operator==(const ResponseTerminateProcess &other) const {
    return error_code == other.error_code;
  }
};
} // namespace Packet