#pragma once
#include "Packet.hpp"
#include "Tcp.hpp"

namespace Network {
class Controller : public TcpClient {
public:
  // Make sure io_context always valid
  Controller(asio::io_context &io_context,
             const asio::ip::tcp::endpoint &endpoint)
      : TcpClient(io_context, endpoint, 0), io_context(io_context),
        endpoint(endpoint) {}
  virtual ~Controller();

  // AbstractClient implementation
  virtual void run() override;

private:
  bool checkSubChannelExist(Bridge::Role role);
  bool createSubChannel(Bridge::Role role, const std::string &id);

  std::unordered_map<Bridge::Role, std::tuple<std::thread *, AbstractClient *>>
      sub_channels;

  asio::io_context &io_context;
  asio::ip::tcp::endpoint endpoint;
};
} // namespace Network