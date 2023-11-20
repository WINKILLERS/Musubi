#ifndef TCP_HANDLER_HPP
#define TCP_HANDLER_HPP
#include "Session.hpp"
#include "qtmetamacros.h"
#include <QTcpServer>

namespace Network {
class TcpHandler : public QTcpServer {
  Q_OBJECT;

public:
  static const uint32_t default_port = 11451;

  TcpHandler(uint32_t port_ = default_port, QObject *parent = nullptr);
  ~TcpHandler();

  inline uint32_t getListeningPort() const { return port; }

  void pause();

  void resume();

  void shutdown();

  bool run();

protected:
  void incomingConnection(qintptr socket_descriptor) override;

private:
  uint32_t port;

  std::vector<Session * /*session*/> pending_queue;

private slots:
  void handleDisconnect();
};
} // namespace Network
#endif