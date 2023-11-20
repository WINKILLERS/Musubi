#include "Session.hpp"
#include "Handshake.hpp"
#include "QtConcurrent/qtconcurrentrun.h"
#include "TcpHandler.hpp"
#include <QtConcurrent/QtConcurrent>
#include <spdlog/spdlog.h>

namespace Network {
Session::Session(TcpHandler *handler_, QAbstractSocket *socket_)
    : handler(handler_), socket(socket_) {
  connect(socket, &QAbstractSocket::disconnected, this, &Session::disconnected);
  connect(socket, &QAbstractSocket::readyRead, this, &Session::appendToBuffer);
}

Session::~Session() { socket->deleteLater(); }

void Session::shutdown() { socket->disconnectFromHost(); }

bool Session::processPacket(std::string raw_packet) {
  const auto description = getDescription();

  // Parse the request
  Bridge::Parser parser;
  if (parser.parseJson(raw_packet) == false) {
    spdlog::error("[{}] error while parsing packet", description);
    return false;
  }

  // Get packet's information
  const auto &header = parser.getHeader();

  // Get header fields
  const auto type = header->type;
  const auto id = header->id;
  const auto timestamp = header->timestamp;

  // Check is client sending server command
  if (PACKET_SERVER_TYPE(type) == true) {
    spdlog::error("[{}] session sended server command", description);
    shutdown();
    return false;
  }

  // Process handshake
  if (type == Bridge::Type::client_handshake) {
    const auto &body = parser.getBody<Bridge::ClientHandshake>();
    hwid = body->hwid;
    role = body->role;

    // If hwid not present, the client is invalid
    if (hwid.empty()) {
      spdlog::error("[{}] session hwid empty", description);
      shutdown();
      return false;
    }

    // If role not set, the client is invalid
    if (role == Bridge::Role::unknown) {
      spdlog::error("[{}] session role error", description);
      shutdown();
      return false;
    }

    // Server internal error
    auto migrate = handler->migratePendingSession(this);
    if (migrate != true) {
      spdlog::error("[{}] session can not migrate", description);
      shutdown();
      return false;
    }

    handshake_id = id;

    return true;
  }

  // If we have not performed handshake
  if (role != Bridge::Role::unknown) {
    spdlog::error("[{}] session sending request before initialization",
                  description);
    shutdown();
    return false;
  }

  return dispatchPacket(parser);
}

bool Session::dispatchPacket(const Bridge::Parser &parser) const {
  // Get packet's information
  const auto &header = parser.getHeader();

  // Get header fields
  auto type = header->type;

  // Dispatch it
  switch (type) {
  default:
    spdlog::error("packet not handled, type: {}", magic_enum::enum_name(type));
    return false;
  };

  return true;
}

void Session::appendToBuffer() {
  const auto description = getDescription();

  // If we are in initial status
  if (bytes_remain == 0 && socket->bytesAvailable() >= 3 * sizeof(uint64_t)) {
    uint64_t magic = 0;

    // Read magic
    socket->read((char *)&magic, sizeof(uint64_t));

    // If magic mismatch
    if (magic != Bridge::getBridgeVersion()) {
      spdlog::error("[{}] invalid packet: magic error (require {} but get {}), "
                    "disposing {} bytes",
                    Bridge::getBridgeVersion(), magic, description,
                    sizeof(uint64_t));

      // Find new header
      if (socket->bytesAvailable() != 0) {
        appendToBuffer();
      }

      return;
    }

    // Get required sizes
    uint64_t header_size = 0;
    uint64_t body_size = 0;

    socket->read((char *)&header_size, sizeof(uint64_t));
    socket->read((char *)&body_size, sizeof(uint64_t));

    // Calculate required size
    bytes_remain = header_size + body_size;

    // If size too big
    if (header_size > max_header_size || body_size > max_body_size) {
      spdlog::error("[{}] invalid packet: packet too large, disposed",
                    description);
      bytes_remain = 0;

      if (socket->bytesAvailable() != 0) {
        appendToBuffer();
      }

      return;
    }

    // Build packet's head
    buffer.reserve(bytes_remain + reverse_padding_size);
    buffer.append((char *)&header_size, sizeof(uint64_t));
    buffer.append((char *)&body_size, sizeof(uint64_t));
  }

  // If we already have required size
  // But we can not read whole packet at once
  if (bytes_remain > socket->bytesAvailable()) {
    auto before = buffer.size();
    auto received = socket->readAll();
    bytes_remain -= received.size();
    buffer.append(received.cbegin(), received.cend());

    return;
  }
  // Or we can read whole packet at once
  else if (bytes_remain != 0) {
    auto before = buffer.size();
    auto received = socket->read(bytes_remain);
    bytes_remain = 0;
    buffer.append(received.cbegin(), received.cend());

    // Call base to process the packet
    auto task =
        QtConcurrent::run(&Session::processPacket, this, std::move(buffer));

    // Clear the buffer
    buffer.clear();
  }

  // If we have available bytes that can calculate required size
  if (socket->bytesAvailable() >= 3 * sizeof(uint64_t)) {
    appendToBuffer();
  }
}
} // namespace Network