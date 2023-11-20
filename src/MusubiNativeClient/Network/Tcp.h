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
  virtual std::optional<Packet::Parser> readJsonPacket() noexcept override;

private:
  // AbstractClient implementation
  virtual bool sendJsonPacketInternal(
      const Packet::AbstractGenerator &packet) noexcept override;

  // Tcp socket
  asio::ip::tcp::socket socket;

  // Server address
  asio::ip::tcp::endpoint endpoint;
};
} // namespace Network