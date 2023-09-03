#pragma once
#include "AbstractPacket.h"

namespace Packet {
class RequestUploadFile : public AbstractPacket {
public:
  std::string remote_path;
  std::string content;

  RequestUploadFile() = default;
  RequestUploadFile(const std::string &remote_path, const std::string &content)
      : remote_path(remote_path), content(content) {}
  RequestUploadFile(const std::string &remote_path, std::string &&content)
      : remote_path(remote_path), content(std::move(content)) {}

  std::string buildJson() const override final;
  void parseJson(const std::string &buffer) override final;

  enum { PacketType = (uint32_t)Type::request_upload_file };
  inline Type getType() const override final { return (Type)PacketType; }

  inline bool operator==(const RequestUploadFile &other) const {
    return remote_path == other.remote_path && content == other.content;
  }
};

class ResponseUploadFile : public AbstractPacket {
public:
  uint32_t error_code;

  ResponseUploadFile() = default;
  ResponseUploadFile(uint32_t error_code) : error_code(error_code) {}

  std::string buildJson() const override final;
  void parseJson(const std::string &buffer) override final;

  enum { PacketType = (uint32_t)Type::response_upload_file };
  inline Type getType() const override final { return (Type)PacketType; }

  inline bool operator==(const ResponseUploadFile &other) const {
    return error_code == other.error_code;
  }
};
} // namespace Packet