#pragma once
#include "AbstractPacket.h"

namespace Packet {
class RequestExecuteFile : public AbstractPacket {
public:
  std::string path;

  RequestExecuteFile() = default;
  RequestExecuteFile(const std::string &path) : path(path) {}

  std::string buildJson() const override final;
  void parseJson(const std::string &buffer) override final;

  enum { PacketType = (uint32_t)Type::request_execute_file };
  inline Type getType() const override final { return (Type)PacketType; }

  inline bool operator==(const RequestExecuteFile &other) const {
    return path == other.path;
  }
};

class ResponseExecuteFile : public AbstractPacket {
public:
  uint64_t error_code;

  ResponseExecuteFile() = default;
  ResponseExecuteFile(uint64_t error_code) : error_code(error_code) {}

  std::string buildJson() const override final;
  void parseJson(const std::string &buffer) override final;

  enum { PacketType = (uint32_t)Type::response_execute_file };
  inline Type getType() const override final { return (Type)PacketType; }

  inline bool operator==(const ResponseExecuteFile &other) const {
    return error_code == other.error_code;
  }
};
} // namespace Packet