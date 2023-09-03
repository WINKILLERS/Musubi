#pragma once
#include "Protocols.h"
#include "Tcp.h"

namespace Network {
class File : public TcpClient {
public:
  File(asio::io_context &io_context, const asio::ip::tcp::endpoint &endpoint,
       const std::string &handshake_id)
      : TcpClient(io_context, endpoint, handshake_id) {}
  virtual ~File();

  // AbstractClient implementation
  virtual void run() noexcept override;

private:
  bool onQuery(std::shared_ptr<Packet::Header> header,
               std::shared_ptr<Packet::AbstractPacket> param) noexcept;
  bool onRemove(std::shared_ptr<Packet::Header> header,
                std::shared_ptr<Packet::AbstractPacket> param) noexcept;
  bool onUpload(std::shared_ptr<Packet::Header> header,
                std::shared_ptr<Packet::AbstractPacket> param) noexcept;
  bool onExecute(std::shared_ptr<Packet::Header> header,
                 std::shared_ptr<Packet::AbstractPacket> param) noexcept;
  bool onDownload(std::shared_ptr<Packet::Header> header,
                  std::shared_ptr<Packet::AbstractPacket> param) noexcept;
};
} // namespace Network