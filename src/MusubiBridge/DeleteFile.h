#pragma once
#include "AbstractPacket.h"

namespace Packet {
class RequestDeleteFile : public AbstractPacket {
public:
  std::string path;

  RequestDeleteFile() = default;
  RequestDeleteFile(const std::string &path) : path(path) {}

  std::string buildJson() const override final;
  void parseJson(const std::string &buffer) override final;

  enum { PacketType = (uint32_t)Type::request_delete_file };
  inline Type getType() const override final { return (Type)PacketType; }

  inline bool operator==(const RequestDeleteFile &other) const {
    return path == other.path;
  }
};

class ResponseDeleteFile : public AbstractPacket {
public:
  uint64_t removed_count;

  ResponseDeleteFile() = default;
  ResponseDeleteFile(const uint64_t &removed_count)
      : removed_count(removed_count) {}

  std::string buildJson() const override final;
  void parseJson(const std::string &buffer) override final;

  enum { PacketType = (uint32_t)Type::response_delete_file };
  inline Type getType() const override final { return (Type)PacketType; }

  inline bool operator==(const ResponseDeleteFile &other) const {
    return removed_count == other.removed_count;
  }
};
} // namespace Packet