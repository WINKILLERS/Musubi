#pragma once
#include "AbstractPacket.h"
#include "chrono"

namespace Packet {
class Header : public AbstractPacket {
public:
  Type type;
  std::string id;
  uint64_t timestamp;
  uint32_t version;

  Header()
      : type(Type::unknown), id("Default"), timestamp(0),
        version(getBridgeVersion()) {
    if (timestamp == 0)
      timestamp = std::chrono::duration_cast<std::chrono::milliseconds>(
                      std::chrono::system_clock::now().time_since_epoch())
                      .count();
  }
  Header(const Type &type, const std::string &id,
         const uint64_t &timestamp_ = 0)
      : type(type), id(id), timestamp(timestamp_), version(getBridgeVersion()) {
    if (timestamp == 0)
      timestamp = std::chrono::duration_cast<std::chrono::milliseconds>(
                      std::chrono::system_clock::now().time_since_epoch())
                      .count();
  }

  std::string buildJson() const override final;
  void parseJson(const std::string &buffer) override final;

  inline void setType(Type type_) { type = type_; }
  enum { PacketType = (uint32_t)Type::header };
  inline Type getType() const override final { return (Type)PacketType; }

  inline bool operator==(const Header &other) const {
    return type == other.type && id == other.id && timestamp == other.timestamp;
  }
};
} // namespace Packet