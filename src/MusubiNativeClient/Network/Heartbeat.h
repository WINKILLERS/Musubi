#pragma once
#include "Protocols.h"
#include "Tcp.h"

using namespace std::chrono_literals;

namespace Network {
class Heartbeat : public TcpClient {
public:
  Heartbeat(asio::io_context &io_context,
            const asio::ip::tcp::endpoint &endpoint,
            const std::string &handshake_id, std::chrono::seconds duration = 3s)
      : TcpClient(io_context, endpoint, handshake_id), duration(duration) {}
  virtual ~Heartbeat();

  // AbstractClient implementation
  virtual void run() noexcept override;

  void setDuration(std::chrono::seconds duration_) noexcept {
    duration = duration_;
  }

private:
  bool sendHeartbeat() noexcept;

  std::chrono::seconds duration;
};
} // namespace Network