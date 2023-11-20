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

void TcpHandler::pause() { pauseAccepting(); }

void TcpHandler::resume() { resumeAccepting(); }

void TcpHandler::shutdown() { close(); }

bool TcpHandler::run() { return QTcpServer::listen(QHostAddress::Any, port); };

void TcpHandler::incomingConnection(qintptr socket_descriptor) {
  auto socket = new QTcpSocket;

  if (socket->setSocketDescriptor(socket_descriptor) == false) {
    spdlog::error("error while accepting connection");
    return;
  }

  auto session = new Session(this, socket);

  pending_queue.emplace_back(session);

  connect(session, &Session::disconnected, this, &TcpHandler::handleDisconnect);
}

Session *TcpHandler::getClient(const std::string &hwid) const {
  try {
    return clients.at(hwid);
  } catch (const std::exception &) {
    // The controller is deleted, we can not find
    return nullptr;
  }
}

bool TcpHandler::migratePendingSession(Session *session) {
  auto description = session->getDescription();

  // Basic check
  if (session->getRole() == Bridge::Role::unknown) {
    spdlog::error("[{}] trying to migrate a uninitialized session",
                  description);
    assert(false);
    return false;
  }

  // Find the session in queue
  auto iter = std::find(pending_queue.cbegin(), pending_queue.cend(), session);
  if (iter == pending_queue.cend()) {
    spdlog::error("[{}] trying to migrate a non-exist session", description);
    assert(false);
    return false;
  }

  // Remove the session from pending queue
  pending_queue.erase(iter);

  spdlog::info("[{}] connected", description);

  // If role is controller
  if (session->getRole() == Bridge::Role::controller) {

    // Add the session to controller
    clients[session->getHwid()] = session;

    // Notify new controller
    emit clientConnected(session);
  }
  // Or we are a channel session
  else {
    // Get the hwid for the session and get controller
    auto controller = getClient(session->getHwid());
  }
  // TODO
  return true;
}

void TcpHandler::handleDisconnect() {
  auto session = qobject_cast<Session *>(sender());
  auto iter = std::find(pending_queue.begin(), pending_queue.end(), session);

  pending_queue.erase(iter);
}
} // namespace Network