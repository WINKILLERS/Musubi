#ifndef PACKET_HPP
#define PACKET_HPP
#include <cstdint>
#include <string>

#define EMPLACE_PARAM(p) packet[#p] = p
#define EXTRACT_PARAM(p) p = packet[#p]
#define IMPLEMENT_AS_PACKET(t)                                                 \
  enum { PacketType = (uint16_t)Type::t };                                     \
  inline Type getType() const override { return (Type)PacketType; };           \
  std::string buildJson() const override;                                      \
  void parseJson(const std::string &json) override;
#define IMPLEMENT_AS_PACKET_DEFAULT(t)                                         \
  enum { PacketType = (uint16_t)Type::t };                                     \
  inline Type getType() const override { return (Type)PacketType; };
#define CASE_AND_EMIT(pt)                                                      \
  case (Bridge::Type)Bridge::pt::PacketType:                                   \
    emit recv##pt(parser.getHeader(), parser.getBody<Bridge::pt>());           \
    break
#define DECLARE_SIGNAL(pt)                                                     \
  void recv##pt(Bridge::HeaderPtr header, std::shared_ptr<Bridge::pt> packet)

namespace Bridge {
uint32_t getBridgeVersion();

enum class Type : uint16_t {
  unknown,
  header = 64,
  client_handshake,
  server_handshake = 1024,
};

enum class Role : uint8_t { unknown, controller };

#define RESERVED_TYPE(t) (t <= Bridge::Type::header)
#define CLIENT_TYPE(t)                                                         \
  (Bridge::Type::client_handshake <= t && t < Bridge::Type::server_handshake)
#define SERVER_TYPE(t) (Bridge::Type::server_handshake <= t)

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