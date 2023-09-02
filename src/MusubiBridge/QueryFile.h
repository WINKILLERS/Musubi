#pragma once
#include "AbstractPacket.h"
#include "vector"

namespace Packet {
class RequestFileChannel : public AbstractPacket {
public:
  RequestFileChannel() = default;

  std::string buildJson() const override final;
  void parseJson(const std::string &buffer) override final;

  enum { PacketType = (uint32_t)Type::request_file_channel };
  inline Type getType() const override final { return (Type)PacketType; }
};

class RequestQueryFile : public AbstractPacket {
public:
  std::string path;

  RequestQueryFile() = default;
  RequestQueryFile(const std::string &path) : path(path) {}
  RequestQueryFile(const std::u8string &path)
      : path(path.begin(), path.end()) {}

  std::string buildJson() const override final;
  void parseJson(const std::string &buffer) override final;

  enum { PacketType = (uint32_t)Type::request_query_file };
  inline Type getType() const override final { return (Type)PacketType; }

  inline bool operator==(const RequestQueryFile &other) const {
    return path == other.path;
  }
};

struct File {
  std::string name;
  bool is_directory;
};

class ResponseQueryFile : public AbstractPacket {
public:
  std::vector<File> list;
  bool is_error;

  ResponseQueryFile() = default;
  ResponseQueryFile(const std::vector<File> &list, const bool &is_error)
      : list(list), is_error(is_error) {}

  std::string buildJson() const override final;
  void parseJson(const std::string &buffer) override final;

  enum { PacketType = (uint32_t)Type::response_query_file };
  inline Type getType() const override final { return (Type)PacketType; }

  inline bool operator==(const ResponseQueryFile &other) const {
    if (is_error != other.is_error)
      return false;

    if (list.size() != other.list.size())
      return false;

    for (size_t i = 0; i < list.size(); i++) {
      auto &t1 = list[i];
      auto &t2 = other.list[i];

      if (t1.name != t2.name)
        return false;
      if (t1.is_directory != t2.is_directory)
        return false;
    }

    return true;
  }
};
} // namespace Packet