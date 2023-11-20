#include "TcpHandler.hpp"
#include <QTcpSocket>
#include <spdlog/spdlog.h>

namespace Network {
TcpHandler::TcpHandler(uint32_t port_, QObject *parent)
    : port(port_), QTcpServer(parent) {}

TcpHandler::~TcpHandler() {
  for (auto &session : pending_queue) {
    session->shutdown();
    session->deleteLater();
  }

  pending_queue.clear();
}

void TcpHandler::pause() { resumeAccepting(); }

void TcpHandler::resume() { pauseAccepting(); }

void TcpHandler::shutdown() { close(); }

bool TcpHandler::run() { return QTcpServer::listen(QHostAddress::Any, port); };

void TcpHandler::incomingConnection(qintptr socket_descriptor) {
  auto socket = new QTcpSocket;

  if (socket->setSocketDescriptor(socket_descriptor) == false) {
    spdlog::error("error while accepting connection");
    return;
  }

  auto session = new Session(socket);

  pending_queue.emplace_back(session);

  connect(session, &Session::disconnected, this, &TcpHandler::handleDisconnect);
}

void TcpHandler::handleDisconnect() {
  auto session = qobject_cast<Session *>(sender());
  auto iter = std::find(pending_queue.begin(), pending_queue.end(), session);

  pending_queue.erase(iter);
}
} // namespace Network