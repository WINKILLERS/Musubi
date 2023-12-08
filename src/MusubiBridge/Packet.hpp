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
/**
 * @brief Get the Bridge version
 *
 * @return uint64_t
 */
uint64_t getBridgeVersion();

enum class Type : uint32_t {
  unknown,
  header = 64,
  client_handshake,
  heartbeat,
  client_information,
  response_get_processes,
  response_terminate_process,
  response_start_process,
  response_get_files,
  response_remove_files,
  server_handshake = 512,
  request_get_processes,
  request_terminate_process,
  request_start_process,
  request_get_files,
  request_remove_files,
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
  /**
   * @brief Build the packet in json format
   *
   * @return std::string
   */
  virtual std::string buildJson() const;

  /**
   * @brief Parse a json, throw exception on error
   *
   * @param json Packet content in json type
   */
  virtual void parseJson(const std::string &json);

  /**
   * @brief Get packet's type
   *
   * @return Type
   */
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
