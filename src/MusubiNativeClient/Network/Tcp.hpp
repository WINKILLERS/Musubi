#ifndef TCP_HPP
#define TCP_HPP
#include "AbstractClient.hpp"

namespace Network {
class TcpClient : public AbstractClient {
public:
  TcpClient(asio::io_context &io_context,
            const asio::ip::tcp::endpoint &endpoint,
            const uint64_t handshake_id);
  virtual ~TcpClient();

  // AbstractClient implementation
  virtual bool connect() override;
  virtual void shutdown() override;

protected:
  // AbstractClient implementation
  virtual bool sendJsonPacket(const Bridge::AbstractGenerator &packet) override;
  virtual std::optional<Bridge::Parser> readJsonPacket() override;

private:
  // Tcp socket
  asio::ip::tcp::socket socket;

  // Server address
  asio::ip::tcp::endpoint endpoint;
};
} // namespace Network
#endif