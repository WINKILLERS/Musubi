#ifndef CONTROLLER_HPP
#define CONTROLLER_HPP
#include "Packet.hpp"
#include "Tcp.hpp"

namespace Network {
class Controller : public TcpClient {
public:
  // Make sure io_context always valid
  Controller(asio::io_context &io_context,
             const asio::ip::tcp::endpoint &endpoint)
      : TcpClient(io_context, endpoint, 0), io_context(io_context) {}
  ~Controller();

  // AbstractClient implementation
  virtual void run() override;

  inline bool isShutdownByServer() const { return shutdown_by_server; }

private:
  bool checkSubChannelExist(Bridge::Role role);
  bool createSubChannel(const Bridge::Role role, const uint64_t id);

  std::unordered_map<Bridge::Role, std::tuple<std::thread *, AbstractClient *>>
      sub_channels;

  asio::io_context &io_context;

  bool shutdown_by_server = false;

private:
  DECLARE_CALLBACK(ServerHandshake);
  DECLARE_CALLBACK(RequestGetProcesses);
  DECLARE_CALLBACK(RequestTerminateProcess);
};
} // namespace Network
#endif