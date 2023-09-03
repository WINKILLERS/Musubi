#pragma once
#include "Generator.h"
#include "Handshake.h"
#include "Parser.h"
#include "asio.hpp"
#include "memory"
#include "sigc++/sigc++.h"
#include "string"

namespace Network {
class AbstractClient {
public:
  AbstractClient(const std::string &handshake_id);
  AbstractClient(const std::string &handshake_id, Packet::Handshake::Role role);
  virtual ~AbstractClient();

  virtual void run() noexcept = 0;

  // Connect to the server
  virtual bool connect() noexcept = 0;
  // Disconnect from the server
  virtual void shutdown() noexcept = 0;

  // Get hwid of the client
  inline std::string getHwid() const noexcept { return hwid; };
  inline std::string getHandshakeId() const noexcept { return handshake_id; };

protected:
  // Send json packet
  virtual bool
  sendJsonPacket(const Packet::AbstractGenerator &packet) noexcept = 0;
  // Wait until a packet received
  virtual std::optional<Packet::Parser> readJsonPacket() noexcept = 0;

  // Perform handshake
  bool performHandshake(Packet::Handshake::Role role =
                            Packet::Handshake::Role::controller) noexcept;

  // Dispatch packet
  void dispatch(const Packet::Parser &parser) noexcept;

  template <typename T_return, typename T_obj, typename T_obj2,
            typename... T_arg>
  inline decltype(auto) registerCallback(Packet::Type type, T_obj &obj,
                                         T_return (T_obj2::*func)(T_arg...));

private:
  // Invoke call back
  bool invoke(Packet::Type type, std::shared_ptr<Packet::Header> header,
              std::shared_ptr<Packet::AbstractPacket> packet) const noexcept;

  std::unordered_map<
      Packet::Type, sigc::signal<bool(std::shared_ptr<Packet::Header>,
                                      std::shared_ptr<Packet::AbstractPacket>)>>
      callbacks;

  // The hwid of the client
  std::string hwid;

  std::string handshake_id;
};

template <typename T_return, typename T_obj, typename T_obj2, typename... T_arg>
inline decltype(auto)
AbstractClient::registerCallback(Packet::Type type, T_obj &obj,
                                 T_return (T_obj2::*func)(T_arg...)) {
  auto &signal = callbacks[type];
  return signal.connect(sigc::mem_fun(obj, func));
}
} // namespace Network