#include "Session.hpp"
#include "Factory.hpp"
#include "Packet.hpp"
#include "QtConcurrent/qtconcurrentrun.h"
#include <QtConcurrent/QtConcurrent>
#include <spdlog/spdlog.h>

namespace Network {
Session::Session(QAbstractSocket *socket_) : socket(socket_) {
  connect(socket, &QAbstractSocket::disconnected, this, &Session::disconnected);
  connect(socket, &QAbstractSocket::readyRead, this, &Session::appendToBuffer);
}

Session::~Session() { socket->deleteLater(); }

void Session::shutdown() { socket->disconnectFromHost(); }

bool Session::processPacket(std::string raw_packet) {
  const auto description = getDescription();

  // Parse the request
  Bridge::Parser parser;
  if (parser.parseJson(buffer) == false) {
    spdlog::error("[{}] error while parsing packet", description);
    return false;
  }
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
      spdlog::error("[{}] invalid packet: magic error, disposing {} bytes",
                    description, sizeof(uint64_t));

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