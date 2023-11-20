#ifndef SESSION_HPP
#define SESSION_HPP
#include "Factory.hpp"
#include "Packet.hpp"
#include <QAbstractSocket>
#include <QHostAddress>
#include <QObject>
#include <format>
#include <magic_enum.hpp>
#include <string>

namespace Network {
class Session : public QObject {
  Q_OBJECT;

public:
  Session(QAbstractSocket *socket_);
  ~Session();

  inline std::string getRemoteAddress() const {
    return socket->peerAddress().toString().toStdString();
  }

  inline uint32_t getRemotePort() const { return socket->peerPort(); }

  inline std::string getDescription() const {
    return std::format("{}:{} {}", getRemoteAddress(), getRemotePort(),
                       magic_enum::enum_name(role));
  }

  std::string getHwid() const { return hwid; }

  void shutdown();

signals:
  void disconnected();

private:
  static const uint32_t max_header_size = 1024;
  static const uint32_t max_body_size = 10ul * 1024 * 1024;
  static const uint32_t reverse_padding_size = 100;

  QAbstractSocket *socket;

  uint64_t handshake_id;
  Bridge::Role role;
  std::string hwid;

  size_t bytes_remain = 0;
  std::string buffer;

  bool processPacket(std::string raw_packet);
  bool dispatchPacket(const Bridge::Parser &parser) const;

private slots:
  void appendToBuffer();
};
} // namespace Network
#endif