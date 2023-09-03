#pragma once
#include "AbstractClient.h"

namespace Network {
class TcpClient : public AbstractClient {
public:
  TcpClient(asio::io_context &io_context,
            const asio::ip::tcp::endpoint &endpoint,
            const std::string &handshake_id);
  virtual ~TcpClient();

  // AbstractClient implementation
  virtual bool connect() noexcept override;
  virtual void shutdown() noexcept override;

protected:
  // AbstractClient implementation
  virtual bool
  sendJsonPacket(const Packet::AbstractGenerator &packet) noexcept override;
  virtual std::optional<Packet::Parser> readJsonPacket() noexcept override;

private:
  // Tcp socket
  asio::ip::tcp::socket socket;

  // Server address
  asio::ip::tcp::endpoint endpoint;
};
} // namespace Network