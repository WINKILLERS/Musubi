#ifndef SESSION_HPP
#define SESSION_HPP
#include <QAbstractSocket>
#include <QHostAddress>
#include <QObject>
#include <format>
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
    return std::format("{}:{}", getRemoteAddress(), getRemotePort());
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

  std::string hwid;

  size_t bytes_remain = 0;
  std::string buffer;

  bool processPacket(std::string raw_packet);

private slots:
  void appendToBuffer();
};
} // namespace Network
#endif