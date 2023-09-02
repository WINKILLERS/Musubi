#pragma once
#include "AbstractPacket.h"
#include "vector"

namespace Packet {
class RequestProgramChannel : public AbstractPacket {
public:
  RequestProgramChannel() = default;

  std::string buildJson() const override final;
  void parseJson(const std::string &buffer) override final;

  enum { PacketType = (uint32_t)Type::request_program_channel };
  inline Type getType() const override final { return (Type)PacketType; }
};

class RequestQueryProgram : public AbstractPacket {
public:
  RequestQueryProgram() = default;

  std::string buildJson() const override final;
  void parseJson(const std::string &buffer) override final;

  inline Type getType() const override final {
    return Type::request_query_program;
  }
};

struct Program {
  std::string name;
  std::string install_path;
  std::string publisher;
  std::string install_date;
  std::string version;
};

class ResponseQueryProgram : public AbstractPacket {
public:
  std::vector<Program> list;

  ResponseQueryProgram() = default;
  ResponseQueryProgram(const std::vector<Program> &list) : list(list) {}

  std::string buildJson() const override final;
  void parseJson(const std::string &buffer) override final;

  enum { PacketType = (uint32_t)Type::response_query_program };
  inline Type getType() const override final { return (Type)PacketType; }

  inline bool operator==(const ResponseQueryProgram &other) const {
    if (list.size() != other.list.size())
      return false;

    for (size_t i = 0; i < list.size(); i++) {
      auto &t1 = list[i];
      auto &t2 = other.list[i];

      if (t1.name != t2.name)
        return false;
      if (t1.install_path != t2.install_path)
        return false;
      if (t1.publisher != t2.publisher)
        return false;
      if (t1.install_date != t2.install_date)
        return false;
      if (t1.version != t2.version)
        return false;
    }

    return true;
  }
};
} // namespace Packet