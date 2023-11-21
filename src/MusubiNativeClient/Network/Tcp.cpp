#include "Tcp.hpp"
#include <magic_enum.hpp>
#include <spdlog/spdlog.h>

namespace Network {
TcpClient::TcpClient(asio::io_context &io_context,
                     const asio::ip::tcp::endpoint &endpoint,
                     const uint64_t handshake_id)
    : AbstractClient(handshake_id), socket(io_context), endpoint(endpoint) {}

Network::TcpClient::~TcpClient() {}

bool Network::TcpClient::connect() {
  spdlog::info("connecting to tcp://{}:{}", endpoint.address().to_string(),
               endpoint.port());

  try {
    socket.connect(endpoint);
  } catch (const asio::system_error &e) {
    spdlog::error("connect error, message: {}", e.what());
    return false;
  }

  return true;
}

void Network::TcpClient::shutdown() {
  try {
    socket.close();
  } catch (const asio::system_error &e) {
    spdlog::error("disconnect error, message: {}", e.what());
  }
}

bool Network::TcpClient::sendJsonPacket(
    const Bridge::AbstractGenerator &packet) {
  try {
    auto magic = Bridge::getBridgeVersion();
    auto buffer = packet.buildJson();

    spdlog::trace("sending packet with type: {}, size: {:.2f} KB",
                  magic_enum::enum_name(packet.getType()),
                  (double)buffer.size() / 1024);

    asio::write(socket, asio::buffer(&magic, sizeof(magic)),
                asio::transfer_exactly(sizeof(magic)));

    asio::write(socket, asio::buffer(buffer),
                asio::transfer_exactly(buffer.size()));
  } catch (const asio::system_error &e) {
    spdlog::error("error while sending packet, message: {}", e.what());
    return false;
  }

  return true;
}

std::optional<Bridge::Parser> Network::TcpClient::readJsonPacket() {
  // Retry infinitely
  while (true) {
    uint64_t magic = 0;

    try {
      while (true) {
        asio::read(socket, asio::buffer(&magic, sizeof(uint64_t)),
                   asio::transfer_exactly(sizeof(uint64_t)));

        if (magic != Bridge::getBridgeVersion()) {
          spdlog::error("invalid packet: magic error (require {} but get {}), "
                        "disposing {} bytes",
                        Bridge::getBridgeVersion(), magic, sizeof(uint64_t));
        } else {
          break;
        }
      }
    } catch (const asio::system_error &e) {
      spdlog::error("error while reading magic, message: {}", e.what());
      return std::nullopt;
    }

    Bridge::Parser parser;
    uint64_t header_size = 0;
    uint64_t body_size = 0;
    std::string header_buffer;
    std::string body_buffer;

    try {
      asio::read(socket, asio::buffer(&header_size, sizeof(uint64_t)),
                 asio::transfer_exactly(sizeof(uint64_t)));

      asio::read(socket, asio::buffer(&body_size, sizeof(uint64_t)),
                 asio::transfer_exactly(sizeof(uint64_t)));
    } catch (const asio::system_error &e) {
      spdlog::error("error while reading packet sizes, message: {}", e.what());
      return std::nullopt;
    }

    header_buffer.resize(header_size);
    body_buffer.resize(body_size);

    try {
      asio::read(socket, asio::buffer(header_buffer, header_size),
                 asio::transfer_exactly(header_size));

      asio::read(socket, asio::buffer(body_buffer, body_size),
                 asio::transfer_exactly(body_size));
    } catch (const asio::system_error &e) {
      spdlog::error("error while reading packet content", e.what());
      return std::nullopt;
    }

    if (parser.parseJson(header_buffer, body_buffer) == false) {
      spdlog::error("error while parsing packet");
      continue;
    }

    return parser;
  }
}
} // namespace Network
