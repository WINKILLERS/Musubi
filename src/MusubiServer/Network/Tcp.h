#pragma once
#include "AbstractProtocol.h"
#include "qtcpserver.h"
#include "qtcpsocket.h"


namespace Network {
class TcpHandler : public QTcpServer, public AbstractHandler {
  Q_OBJECT

public:
  static const uint32_t default_port = 11451;

  explicit TcpHandler(uint16_t port = default_port, QObject *parent = nullptr);
  virtual ~TcpHandler();

  // AbstractHandler implementation
  virtual void pause() noexcept override { resumeAccepting(); };
  virtual void resume() noexcept override { pauseAccepting(); };
  virtual void shutdown() noexcept override { close(); }
  virtual bool listen() noexcept override {
    return QTcpServer::listen(QHostAddress::Any, port);
  };
  virtual void deleteSelf() noexcept override { deleteLater(); };

protected:
  // Reimplementation for QTcpServer
  void incomingConnection(qintptr socket_descriptor) override;

private slots:
  void onSessionDisconnected();

private:
  const uint16_t port = default_port;
};

class TcpSession : public QTcpSocket, public AbstractSession {
  Q_OBJECT

public:
  static const uint32_t max_header_size = 1024;
  static const uint32_t max_body_size = 10ul * 1024 * 1024;
  static const uint32_t reverse_padding_size = 100;

  TcpSession(int32_t socket_descriptor, TcpHandler *handler);
  virtual ~TcpSession();

  // AbstractSession implementation
  virtual std::string getRemoteAddress() const noexcept override {
    return peerAddress().toString().toStdString();
  }
  virtual uint16_t getRemotePort() const noexcept override {
    return peerPort();
  }
  virtual bool
  sendJsonPacket(const Packet::AbstractGenerator &packet) noexcept override;
  virtual void shutdown() noexcept override;
  virtual void deleteSelf() noexcept override { deleteLater(); };

private slots:
  void appendToBuffer();

private:
  void appendToBufferInternal(uint32_t depth);

  size_t bytes_remain = 0;
  std::string buffer;
};
} // namespace Network