#pragma once
#include "Protocols.h"
#include "Tcp.h"


namespace Network {
class Controller : public TcpClient {
public:
  // Make sure io_context always valid
  Controller(asio::io_context &io_context,
             const asio::ip::tcp::endpoint &endpoint)
      : TcpClient(io_context, endpoint, ""), io_context(io_context),
        endpoint(endpoint) {}
  virtual ~Controller();

  // AbstractClient implementation
  virtual void run() noexcept override;

private:
  bool onInformation(std::shared_ptr<Packet::Header> header,
                     std::shared_ptr<Packet::AbstractPacket> param) noexcept;
  bool onGetProcess(std::shared_ptr<Packet::Header> header,
                    std::shared_ptr<Packet::AbstractPacket> param) noexcept;
  bool
  onHeartbeatChannel(std::shared_ptr<Packet::Header> header,
                     std::shared_ptr<Packet::AbstractPacket> param) noexcept;
  bool onFileChannel(std::shared_ptr<Packet::Header> header,
                     std::shared_ptr<Packet::AbstractPacket> param) noexcept;
  bool onProgramChannel(std::shared_ptr<Packet::Header> header,
                        std::shared_ptr<Packet::AbstractPacket> param) noexcept;
  bool onScreenChannel(std::shared_ptr<Packet::Header> header,
                       std::shared_ptr<Packet::AbstractPacket> param) noexcept;

  bool checkSubchannelExist(Packet::Handshake::Role role) noexcept;
  bool createSubchannel(Packet::Handshake::Role role,
                        const std::string &id) noexcept;

  std::unordered_map<Packet::Handshake::Role,
                     std::tuple<std::thread *, AbstractClient *>>
      sub_channels;

  asio::io_context &io_context;
  asio::ip::tcp::endpoint endpoint;
};
} // namespace Network