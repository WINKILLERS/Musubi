#ifndef SESSION_HPP
#define SESSION_HPP
#include "Factory.hpp"
#include "Handshake.hpp"
#include "Heartbeat.hpp"
#include "Packet.hpp"
#include <QAbstractSocket>
#include <QHostAddress>
#include <QObject>
#include <format>
#include <magic_enum.hpp>
#include <optional>
#include <string>
#include <vector>

#define CASE_AND_EMIT(pt)                                                      \
  case (Bridge::Type)Bridge::pt::PacketType:                                   \
    emit recv##pt(parser.getHeader(), parser.getBody<Bridge::pt>());           \
    break

#define DECLARE_SIGNAL(pt)                                                     \
  void recv##pt(Bridge::HeaderPtr header, std::shared_ptr<Bridge::pt> packet)  \
      const

#define DECLARE_SLOT(pt)                                                       \
  void on##pt(Bridge::HeaderPtr header, std::shared_ptr<Bridge::pt> packet)

namespace Network {
class Handler;

class Session : public QObject {
  Q_OBJECT;
  friend Handler;

public:
  Session(Handler *handler_, QAbstractSocket *socket_,
          Session *parent_ = nullptr);
  ~Session();

  inline std::string getRemoteAddress() const {
    return socket->peerAddress().toString().toStdString();
  }

  inline uint32_t getRemotePort() const { return socket->peerPort(); }

  inline std::string getDescription() const {
    return std::format("{}:{} {}", getRemoteAddress(), getRemotePort(),
                       magic_enum::enum_name(role));
  }

  inline std::string getHwid() const { return hwid; }

  inline Bridge::Role getRole() const { return role; }

  bool sendJsonPacket(const Bridge::AbstractGenerator &packet);

  void shutdown();

signals:
  void disconnected();

  DECLARE_SIGNAL(ClientInformation);
  DECLARE_SIGNAL(Heartbeat);

private:
  // Called from handler when a sub channel connected
  void addSubChannel(Session *sub_channel);

  static const uint32_t max_header_size = 1024;
  static const uint32_t max_body_size = 10ul * 1024 * 1024;
  static const uint32_t reverse_padding_size = 100;

  QAbstractSocket *socket = nullptr;
  Handler *handler = nullptr;

  uint64_t handshake_id = 0;
  Bridge::Role role = Bridge::Role::unknown;
  std::string hwid;
  std::optional<Bridge::ClientInformation> information;

  size_t bytes_remain = 0;
  std::string buffer;

  Session *parent;
  std::vector<Session *> sub_channels;

  bool processPacket(std::string raw_packet);
  bool dispatchPacket(const Bridge::Parser &parser) const;

private slots:
  void appendToBuffer();
  void handleSubChannelDisconnect();

  DECLARE_SLOT(ClientInformation);
};
} // namespace Network
#endif