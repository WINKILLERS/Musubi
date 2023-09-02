#pragma once
#include "AbstractPacket.h"

namespace Packet {
class RequestDownloadFile : public AbstractPacket {
public:
  std::string path;

  RequestDownloadFile() = default;
  RequestDownloadFile(const std::string &path) : path(path) {}
  RequestDownloadFile(const std::u8string &path)
      : path(path.begin(), path.end()) {}

  std::string buildJson() const override final;
  void parseJson(const std::string &buffer) override final;

  enum { PacketType = (uint32_t)Type::request_download_file };
  inline Type getType() const override final { return (Type)PacketType; }

  inline bool operator==(const RequestDownloadFile &other) const {
    return path == other.path;
  }
};

class ResponseDownloadFile : public AbstractPacket {
public:
  uint32_t error_code;
  std::string content;

  ResponseDownloadFile() = default;
  ResponseDownloadFile(const uint32_t &error_code, const std::string &content)
      : error_code(error_code), content(content) {}

  std::string buildJson() const override final;
  void parseJson(const std::string &buffer) override final;

  enum { PacketType = (uint32_t)Type::response_download_file };
  inline Type getType() const override final { return (Type)PacketType; }

  inline bool operator==(const ResponseDownloadFile &other) const {
    return error_code == other.error_code && content == other.content;
  }
};
} // namespace Packet