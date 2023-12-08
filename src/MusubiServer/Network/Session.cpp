#include "Session.hpp"
#include "Handler.hpp"
#include "magic_enum.hpp"
#include <QWidget>
#include <QtConcurrent/QtConcurrent>
#include <spdlog/spdlog.h>

namespace Network {
Session::Session(Handler *handler_, QAbstractSocket *socket_, Session *parent_)
    : handler(handler_), socket(socket_), parent(parent_), QObject(parent_) {
  socket->setSocketOption(QAbstractSocket::KeepAliveOption, 1);

  connect(socket, &QAbstractSocket::disconnected, this, &Session::disconnected);
  connect(socket, &QAbstractSocket::readyRead, this, &Session::appendToBuffer);

  connect(this, &Session::recvClientInformation, this,
          &Session::onClientInformation);
  connect(this, &Session::recvResponseGetProcesses, this,
          &Session::onResponseGetProcesses);

  connect(this, &Session::migratePendingSession, handler,
          &Handler::migratePendingSession);
}

Session::~Session() {
  auto sub_channels_ = sub_channels;
  socket->deleteLater();

  for (auto &sub_channel : sub_channels_) {
    sub_channel->shutdown();
    // Qt will delete sub_channel for us
  }

  closeAllWindow();
}

bool Session::sendJsonPacket(const Bridge::AbstractGenerator &packet) {
  const auto description = getDescription();
  auto magic = Bridge::getBridgeVersion();
  auto buffer = packet.buildJson();
  auto type = packet.getType();

  spdlog::trace("[{}] sending packet with type: {}, size: {:.2f} KB",
                description, magic_enum::enum_name(type),
                (double)buffer.size() / 1024);

  // Make sure we are in current thread

  // Send magic
  auto ret = socket->write((char *)&magic, sizeof(magic));
  if (ret == false) {
    return false;
  }

  // Send the data, if error occurred, writeData returns -1
  ret = socket->write(buffer.data(), buffer.size()) != -1;

  if (ret == false) {
    return false;
  }

  return socket->flush();
}

void Session::shutdown() {
  socket->disconnectFromHost();
  closeAllWindow();
}

void Session::closeAllWindow() {
  for (auto &window : windows) {
    QMetaObject::invokeMethod(window, &QWidget::close);
    QMetaObject::invokeMethod(window, &QWidget::deleteLater);
  }

  windows.clear();
}

void Session::addSubChannel(Session *sub_channel) {
  sub_channels.emplace_back(sub_channel);
  sub_channel->setParent(this);

  connect(sub_channel, &Session::disconnected, this,
          &Session::handleSubChannelDisconnect);
}

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

  spdlog::trace("[{}] received packet type: {}", description,
                magic_enum::enum_name(type));

  // Check is client sending server command
  if (PACKET_CLIENT_TYPE(type) == false) {
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

    emit migratePendingSession();

    bool send = sendJsonPacket(GENERATE_PACKET(
        Bridge::ServerHandshake, "Official Musubi Server", false));
    if (send != true) {
      spdlog::error("[{}] session can not reply", description);
      shutdown();
      return false;
    }

    refreshProcesses();

    handshake_id = id;

    return true;
  }

  // If we have not performed handshake
  if (role == Bridge::Role::unknown) {
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
    CASE_AND_EMIT(ClientInformation);
    CASE_AND_EMIT(Heartbeat);
    CASE_AND_EMIT(ResponseGetProcesses);
    CASE_AND_EMIT(ResponseTerminateProcess);
    CASE_AND_EMIT(ResponseStartProcess);
    CASE_AND_EMIT(ResponseGetFiles);
    CASE_AND_EMIT(ResponseRemoveFiles);
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
    // auto task =
    //    QtConcurrent::run(&Session::processPacket, this, std::move(buffer));
    processPacket(std::move(buffer));

    // Clear the buffer
    buffer.clear();
  }

  // If we have available bytes that can calculate required size
  if (socket->bytesAvailable() >= 3 * sizeof(uint64_t)) {
    appendToBuffer();
  }
}

void Session::onClientInformation(
    Bridge::HeaderPtr header,
    std::shared_ptr<Bridge::ClientInformation> packet) {
  info = *packet;
}

void Session::onResponseGetProcesses(
    Bridge::HeaderPtr header,
    std::shared_ptr<Bridge::ResponseGetProcesses> packet) {
  auto &unsorted = packet->processes;

  std::sort(unsorted.begin(), unsorted.end(),
            [](const Bridge::Process &p1, const Bridge::Process &p2) {
              auto lower_p1 = p1.name;
              auto lower_p2 = p2.name;
              auto tolower = [](auto c) { return std::tolower(c); };
              std::transform(lower_p1.begin(), lower_p1.end(), lower_p1.begin(),
                             tolower);
              std::transform(lower_p2.begin(), lower_p2.end(), lower_p2.begin(),
                             tolower);

              return lower_p1 < lower_p2;
            });

  processes = *packet;
}

void Session::handleSubChannelDisconnect() {
  auto sub_channel = qobject_cast<Session *>(sender());
  auto description = sub_channel->getDescription();

  spdlog::info("[{}] disconnected", description);

  auto iter = std::find(sub_channels.begin(), sub_channels.end(), sub_channel);
  sub_channels.erase(iter);

  sub_channel->deleteLater();
}
} // namespace Network
