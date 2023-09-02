#pragma once
#include "AbstractPacket.h"
#include "vector"

namespace Packet {
class RequestGetProcess : public AbstractPacket {
public:
  RequestGetProcess() = default;

  std::string buildJson() const override final;
  void parseJson(const std::string &buffer) override final;

  enum { PacketType = (uint32_t)Type::request_get_process };
  inline Type getType() const override final { return (Type)PacketType; }
};

struct Process {
  std::string name;
  uint64_t pid;
  uint64_t ppid;
};

class ResponseGetProcess : public AbstractPacket {
public:
  std::vector<Process> list;

  ResponseGetProcess() = default;
  ResponseGetProcess(const std::vector<Process> &list) : list(list) {}

  std::string buildJson() const override final;
  void parseJson(const std::string &buffer) override final;

  enum { PacketType = (uint32_t)Type::response_get_process };
  inline Type getType() const override final { return (Type)PacketType; }

  inline bool operator==(const ResponseGetProcess &other) const {
    if (list.size() != other.list.size())
      return false;

    for (size_t i = 0; i < list.size(); i++) {
      auto &t1 = list[i];
      auto &t2 = other.list[i];

      if (t1.name != t2.name)
        return false;
      if (t1.pid != t2.pid)
        return false;
      if (t1.ppid != t2.ppid)
        return false;
    }

    return true;
  }
};
} // namespace Packet