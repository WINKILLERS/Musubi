#ifndef PACKET_HPP
#define PACKET_HPP
#include <cstdint>
#include <string>

#define EMPLACE_PARAM(p) packet[#p] = p
#define EXTRACT_PARAM(p) p = packet[#p]
#define IMPLEMENT_AS_PACKET(t)                                                 \
  enum { PacketType = (uint32_t)Type::t };                                     \
  inline Type getType() const override { return (Type)PacketType; };           \
  std::string buildJson() const override;                                      \
  void parseJson(const std::string &json) override;
#define IMPLEMENT_AS_PACKET_DEFAULT(t)                                         \
  enum { PacketType = (uint32_t)Type::t };                                     \
  inline Type getType() const override { return (Type)PacketType; };

namespace Bridge {
uint64_t getBridgeVersion();

enum class Type : uint32_t {
  unknown,
  header = 64,
  client_handshake,
  heartbeat,
  client_information,
  server_handshake = 1024,
  max_type
};

enum class Role : uint8_t { unknown, controller, heartbeat };

#define PACKET_RESERVED_TYPE(t) (t <= Bridge::Type::header)
#define PACKET_CLIENT_TYPE(t)                                                  \
  (Bridge::Type::client_handshake <= t && t < Bridge::Type::server_handshake)
#define PACKET_SERVER_TYPE(t)                                                  \
  (Bridge::Type::server_handshake <= t && t < Bridge::Type::max_type)

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
  uint64_t id;
  // Auto calculated timestamp
  uint64_t timestamp;
  // Version of bridge
  uint32_t version;

  Header()
      : type(Type::unknown), id(0), timestamp(0), version(getBridgeVersion()) {
    initializeTimestamp();
  }

  Header(Type type_, const uint64_t id_)
      : type(type_), id(id_), version(getBridgeVersion()) {
    initializeTimestamp();
  };

private:
  // Initialize timestamp field in current time
  void initializeTimestamp();
};
} // namespace Bridge
#endif