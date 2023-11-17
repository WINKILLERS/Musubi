#ifndef TCP_HANDLER_HPP
#define TCP_HANDLER_HPP
#include "Network/Abstract/AbstractHandler.hpp"
#include <QTcpServer>

namespace Network {
class TcpHandler : public QTcpServer, public AbstractHandler {
  Q_OBJECT;

public:
  static const uint32_t default_port = 11451;

  TcpHandler(uint32_t port_ = default_port, QObject *parent = nullptr);

  inline uint32_t getListeningPort() const { return port; }

  // AbstractHandler implementation
  virtual void pause() override;
  virtual void resume() override;
  virtual void shutdown() override;
  virtual bool run() override;

protected:
  // Re-implementation for QTcpServer
  void incomingConnection(qintptr socket_descriptor) override;

private:
  uint32_t port;
};
} // namespace Network
#endif