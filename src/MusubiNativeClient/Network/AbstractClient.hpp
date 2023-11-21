#ifndef ABSTRACT_CLIENT_HPP
#define ABSTRACT_CLIENT_HPP
#include "Factory.hpp"
#include <asio.hpp>
#include <cassert>
#include <sigc++/sigc++.h>
#include <string>

#define REGISTER_CALLBACK(t, c)                                                \
  registerCallback((Bridge::Type)Bridge::t::PacketType, *this, &c::on##t);

#define DECLARE_CALLBACK(t) bool on##t(const Bridge::Parser &parser);
#define GET_BODY(t) const auto body = parser.getBody<Bridge::t>();

namespace Network {
class AbstractClient {
public:
  AbstractClient(const uint64_t handshake_id);
  virtual ~AbstractClient();

  virtual void run() = 0;

  // Connect to the server
  virtual bool connect() = 0;
  // Disconnect from the server
  virtual void shutdown() = 0;

  // Get hwid of the client
  inline std::string getHwid() const { return hwid; };

  inline uint64_t getHandshakeId() const { return handshake_id; };

protected:
  // Send json packet
  virtual bool sendJsonPacket(const Bridge::AbstractGenerator &packet) = 0;
  // Wait until a packet received
  virtual std::optional<Bridge::Parser> readJsonPacket() = 0;

  // Perform handshake
  bool performHandshake(Bridge::Role role);

  // Dispatch packet
  void dispatch(const Bridge::Parser &parser);

  template <typename T_return, typename T_obj, typename T_obj2,
            typename... T_arg>
  inline decltype(auto) registerCallback(const Bridge::Type type, T_obj &obj,
                                         T_return (T_obj2::*func)(T_arg...));

private:
  // Invoke call back
  bool invoke(Bridge::Type type, const Bridge::Parser &parser) const;

  std::unordered_map<Bridge::Type, sigc::signal<bool(const Bridge::Parser &)>>
      callbacks;

  // The hwid of the client
  std::string hwid;

  const uint64_t handshake_id;
};

template <typename T_return, typename T_obj, typename T_obj2, typename... T_arg>
inline decltype(auto)
AbstractClient::registerCallback(const Bridge::Type type, T_obj &obj,
                                 T_return (T_obj2::*func)(T_arg...)) {
  assert(PACKET_SERVER_TYPE(type) == true);

  auto &signal = callbacks[type];
  return signal.connect(sigc::mem_fun(obj, func));
}
} // namespace Network
#endif