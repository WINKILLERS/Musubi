#include "Tcp.h"
#include "AApch.h"

Network::TcpClient::TcpClient(asio::io_context &io_context,
                              const asio::ip::tcp::endpoint &endpoint,
                              const std::string &handshake_id)
    : AbstractClient(handshake_id), socket(io_context), endpoint(endpoint) {}

Network::TcpClient::~TcpClient() {
  if (socket.is_open() == false) {
    return;
  }

  try {
    socket.close();
  } catch (const asio::system_error &e) {
    spdlog::error("disconnect tcp error, what: {}", e.what());
  }
}

bool Network::TcpClient::connect() noexcept {
  spdlog::info("connecting tcp to {}:{}", endpoint.address().to_string(),
               endpoint.port());

  try {
    socket.connect(endpoint);
  } catch (const asio::system_error &e) {
    spdlog::error("connect error, what: {}", e.what());
    return false;
  }

  return true;
}

void Network::TcpClient::shutdown() noexcept {
  try {
    socket.close();
  } catch (const asio::system_error &e) {
    spdlog::error("disconnect error, what: {}", e.what());
  }
}

bool Network::TcpClient::sendJsonPacket(
    const Packet::AbstractGenerator &packet) noexcept {
  try {
    auto buffer = packet.buildJson();
    spdlog::debug("sending packet with type: {}, size: {} kb",
                  magic_enum::enum_name(packet.header.type),
                  (double)buffer.size() / 1024);
    asio::write(socket, asio::buffer(buffer),
                asio::transfer_exactly(buffer.size()));
  } catch (const asio::system_error &e) {
    spdlog::error("connect error, what: {}", e.what());
    return false;
  }

  return true;
}

std::optional<Packet::Parser> Network::TcpClient::readJsonPacket() noexcept {
  // Retry infinitely
  while (true) {
    uint64_t magic = 0;

    try {
      while (true) {
        asio::read(socket, asio::buffer(&magic, sizeof(uint64_t)),
                   asio::transfer_exactly(sizeof(uint64_t)));

        if (magic != Packet::AbstractGenerator::magic) {
          spdlog::error("invalid packet, magic error, ignoring {} bytes",
                        sizeof(uint64_t));
        } else {
          break;
        }
      }
    } catch (const asio::system_error &e) {
      spdlog::error("read error, what: {}", e.what());
      return std::nullopt;
    }

    Packet::Parser parser;
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
      spdlog::error("read error, what: {}", e.what());
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
      spdlog::error("read error, what: {}", e.what());
      return std::nullopt;
    }

    if (parser.parseJson(header_buffer, body_buffer) == false) {
      spdlog::error("received unknown packet");
      continue;
    }

    return parser;
  }
}