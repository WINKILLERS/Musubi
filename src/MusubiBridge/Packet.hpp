#ifndef PACKET_HPP
#define PACKET_HPP

#include <cstdint>
#include <string>

#define EMPLACE_PARAM(p) packet[#p] = p
#define EXTRACT_PARAM(p) p = packet[#p]
#define IMPLEMENT_AS_PACKET(t)                                                 \
  enum { PacketType = (uint8_t)Type::t };                                      \
  inline Type getType() const override { return (Type)PacketType; };           \
  std::string buildJson() const override;                                      \
  void parseJson(const std::string &json) override;
#define IMPLEMENT_AS_PACKET_DEFAULT(t)                                         \
  enum { PacketType = (uint8_t)Type::t };                                      \
  inline Type getType() const override { return (Type)PacketType; };

namespace Bridge {
uint32_t getBridgeVersion();

enum class Type : uint8_t {
  unknown,
  header,
  client_handshake,
  server_handshake
};

class AbstractPacket {
public:
  // Build the json packet
  virtual std::string buildJson() const;

  // Parse a json, throw exception on error
  virtual void parseJson(const std::string &json);

  // Get packet's type
  virtual inline Type getType() const = 0;
};

class Header : public AbstractPacket {
public:
  IMPLEMENT_AS_PACKET(header);

  // Type of body
  Type type;
  // Packet id
  std::string id;
  // Auto calculated timestamp
  uint64_t timestamp;
  // Version of bridge
  uint32_t version;

  Header()
      : type(Type::unknown), id("Unknown"), timestamp(0),
        version(getBridgeVersion()) {
    initializeTimestamp();
  }

  Header(Type type_, const std::string &id_)
      : type(type_), id(id_), version(getBridgeVersion()) {
    initializeTimestamp();
  };

private:
  // Initialize timestamp field in current time
  void initializeTimestamp();
};
} // namespace Bridge
#endif