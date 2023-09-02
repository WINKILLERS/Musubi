#pragma once
#include "AbstractPacket.h"

namespace Packet {
class RequestStartProcess : public AbstractPacket {
public:
  std::string path;

  RequestStartProcess() = default;
  RequestStartProcess(const std::string &path) : path(path) {}

  std::string buildJson() const override final;
  void parseJson(const std::string &buffer) override final;

  enum { PacketType = (uint32_t)Type::request_start_process };
  inline Type getType() const override final { return (Type)PacketType; }

  inline bool operator==(const RequestStartProcess &other) const {
    return path == other.path;
  }
};

class ResponseStartProcess : public AbstractPacket {
public:
  uint32_t error_code;

  ResponseStartProcess() = default;
  ResponseStartProcess(const uint32_t &error_code) : error_code(error_code) {}

  std::string buildJson() const override final;
  void parseJson(const std::string &buffer) override final;

  enum { PacketType = (uint32_t)Type::response_start_process };
  inline Type getType() const override final { return (Type)PacketType; }

  inline bool operator==(const ResponseStartProcess &other) const {
    return error_code == other.error_code;
  }
};
} // namespace Packet