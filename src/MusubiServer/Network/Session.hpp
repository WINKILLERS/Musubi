#ifndef SESSION_HPP
#define SESSION_HPP
#include "Factory.hpp"
#include "GetProcesses.hpp"
#include "Handshake.hpp"
#include "Heartbeat.hpp"
#include "Packet.hpp"
#include "qwidget.h"
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

  inline std::string getComputerName() const {
    return info.value_or(Bridge::ClientInformation()).computer_name;
  }

  inline std::string getUserName() const {
    return info.value_or(Bridge::ClientInformation()).user_name;
  }

  inline std::string getCpuModel() const {
    return info.value_or(Bridge::ClientInformation()).cpu_model;
  }

  inline std::string getOsName() const {
    return info.value_or(Bridge::ClientInformation()).os_name;
  }

  inline std::vector<Bridge::Process> getProcesses() const {
    return processes.value_or(Bridge::ResponseGetProcesses()).processes;
  }

  inline void addWindow(QWidget *window) { windows.emplace_back(window); }

signals:
  void migratePendingSession();

  void disconnected();

  DECLARE_SIGNAL(ClientInformation);
  DECLARE_SIGNAL(Heartbeat);
  DECLARE_SIGNAL(ResponseGetProcesses);

public slots:
  bool sendJsonPacket(const Bridge::AbstractGenerator &packet);

  void shutdown();

  void closeAllWindow();

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
  std::optional<Bridge::ClientInformation> info;
  std::optional<Bridge::ResponseGetProcesses> processes;

  size_t bytes_remain = 0;
  std::string buffer;

  Session *parent;
  std::vector<Session *> sub_channels;

  std::vector<QWidget *> windows;

  bool processPacket(std::string raw_packet);
  bool dispatchPacket(const Bridge::Parser &parser) const;

private slots:
  void appendToBuffer();
  void handleSubChannelDisconnect();

  DECLARE_SLOT(ClientInformation);
  DECLARE_SLOT(ResponseGetProcesses);
};
} // namespace Network
#endif