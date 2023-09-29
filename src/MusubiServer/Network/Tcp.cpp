#include "Tcp.h"
#include "AApch.h"
#include "Protocols.h"

Network::TcpHandler::TcpHandler(uint16_t port, QObject *parent)
    : AbstractHandler(), QTcpServer(parent), port(port) {
  spdlog::debug("initializing tcp handler with port: {}", port);
}

Network::TcpHandler::~TcpHandler() {
  spdlog::debug("shutting down tcp handler");

  // Stop accept
  close();

  // Disconnect all slots, to prevent incoming disconnect notify after dtor
  disconnect();
}

void Network::TcpHandler::incomingConnection(qintptr socket_descriptor) {
  // New connection established
  TcpSession *session = new TcpSession(socket_descriptor, this);
  addPendingSession(session);

  // Connect private slots
  connect(session, &TcpSession::disconnected, this,
          &Network::TcpHandler::onSessionDisconnected);
}

void Network::TcpHandler::onSessionDisconnected() {
  // Get the session disconnected
  auto session = qobject_cast<TcpSession *>(sender());

  // Call the base to remove it
  removeSession(session);
}

Network::TcpSession::TcpSession(int32_t socket_descriptor,
                                Network::TcpHandler *handler)
    : AbstractSession(handler), QTcpSocket(handler) {
  // Connect socket
  setSocketDescriptor(socket_descriptor);

  // Set read buffer size
  setReadBufferSize((uint64_t)max_body_size + max_header_size);

  // Keep tcp alive
  setSocketOption(QAbstractSocket::KeepAliveOption, 1);

  // Connect signals and slots
  connect(this, &QTcpSocket::readyRead, this,
          &Network::TcpSession::appendToBuffer);
  connect(this, &TcpSession::disconnected, &notifier,
          &Network::IPacketNotify::disconnected);
}

Network::TcpSession::~TcpSession() {
  disconnectFromHost();

  bytes_remain = 0;
  buffer.clear();
}

void Network::TcpSession::shutdown() noexcept { disconnectFromHost(); }

void Network::TcpSession::appendToBuffer() {
  try {
    appendToBufferInternal(0);
  }
  // If unhandled error occurred, terminate the session
  catch (const std::exception &) {
    shutdown();
  } catch (...) {
    shutdown();
  }
}

bool Network::TcpSession::sendJsonPacketInternal(
    const Packet::AbstractGenerator &packet) noexcept {
  try {
    spdlog::debug("sending packet with type: {} for {}",
                  magic_enum::enum_name(packet.header.type), fmt::ptr(this));

    auto buffer = packet.buildJson();
    // Send the data, if error occurred, writeData returns -1
    return writeData(buffer.data(), buffer.size()) != -1;
  }
  // If unhandled error occurred, terminate the session
  catch (const std::exception &) {
    shutdown();
    return false;
  } catch (...) {
    shutdown();
    return false;
  }
}

void Network::TcpSession::appendToBufferInternal(uint32_t depth) {
  // If we are in initial status
  if (bytes_remain == 0 && bytesAvailable() >= 3 * sizeof(uint64_t)) {
    uint64_t magic = 0;

    // Read magic
    read((char *)&magic, sizeof(uint64_t));

    // If magic mismatch
    if (magic != Packet::AbstractGenerator::magic) {
      spdlog::error("invalid packet, magic error, ignoring {} bytes",
                    sizeof(uint64_t));

      // Find new header
      if (bytesAvailable() != 0) {
        appendToBufferInternal(depth + 1);
      }

      return;
    }

    // Get required sizes
    uint64_t header_size = 0;
    uint64_t body_size = 0;

    read((char *)&header_size, sizeof(uint64_t));
    read((char *)&body_size, sizeof(uint64_t));

    // Calculate required size
    bytes_remain = header_size + body_size;

    // If size too big
    if (header_size > max_header_size || body_size > max_body_size) {
      spdlog::error("packet too large, disposed");
      bytes_remain = 0;

      if (bytesAvailable() != 0) {
        appendToBufferInternal(depth + 1);
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
  if (bytes_remain > bytesAvailable()) {
    auto before = buffer.size();
    auto received = readAll();
    bytes_remain -= received.size();
    buffer.append(received.cbegin(), received.cend());

    return;
  }
  // Or we can read whole packet at once
  else if (bytes_remain != 0) {
    auto before = buffer.size();
    auto received = read(bytes_remain);
    bytes_remain = 0;
    buffer.append(received.cbegin(), received.cend());

    // Call base to process the packet
    onReceivedPacket(buffer);

    // Clear the buffer
    buffer.clear();

    if (depth != 0) {
      return;
    }
  }

  // If we have available bytes that can calculate required size
  if (bytesAvailable() >= 3 * sizeof(uint64_t)) {
    appendToBufferInternal(depth + 1);
  }
}