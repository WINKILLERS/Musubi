#ifndef HEARTBEAT_CHANNEL_HPP
#define HEARTBEAT_CHANNEL_HPP
#include "Tcp.hpp"

namespace Network {
class HeartbeatChannel : public TcpClient {
public:
  // Make sure io_context always valid
  HeartbeatChannel(asio::io_context &io_context,
                   const asio::ip::tcp::endpoint &endpoint,
                   const uint64_t handshake_id)
      : TcpClient(io_context, endpoint, handshake_id), io_context(io_context) {}
  ~HeartbeatChannel() = default;

  // AbstractClient implementation
  virtual void run() override;

private:
  asio::io_context &io_context;
};
} // namespace Network
#endif