#include "Handler.hpp"
#include <QTcpSocket>
#include <QThread>
#include <spdlog/spdlog.h>

namespace Network {
Handler::Handler(uint32_t port_, QObject *parent)
    : port(port_), QTcpServer(parent) {}

Handler::~Handler() {
  for (auto &session : pending_queue) {
    session->shutdown();
    session->deleteLater();
  }

  pending_queue.clear();
}

void Handler::pause() { pauseAccepting(); }

void Handler::resume() { resumeAccepting(); }

void Handler::shutdown() { close(); }

bool Handler::run() { return QTcpServer::listen(QHostAddress::Any, port); };

void Handler::incomingConnection(qintptr socket_descriptor) {
  auto socket = new QTcpSocket;

  if (socket->setSocketDescriptor(socket_descriptor) == false) {
    spdlog::error("error while accepting connection");
    return;
  }

  auto session = new Session(this, socket);

  pending_queue.emplace_back(session);

  connect(session, &Session::disconnected, this,
          &Handler::handleClientDisconnect);
}

Session *Handler::getClient(const std::string &hwid) const {
  try {
    return clients.at(hwid);
  } catch (const std::exception &) {
    // The controller is deleted, we can not find
    return nullptr;
  }
}

bool Handler::migratePendingSession(Session *session) {
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

  // If role is controller
  if (session->getRole() == Bridge::Role::controller) {
    // Check if this is a duplicated connection
    if (getClient(session->getHwid()) != nullptr) {
      // Multiple connection, shutdown
      return false;
    }

    // Add the session to controller
    clients[session->getHwid()] = session;

    // Notify new controller
    emit clientConnected(session);
  }
  // Or we are a channel session
  else {
    // Get the hwid for the session and get controller
    auto controller = getClient(session->getHwid());

    controller->addSubChannel(session);
  }

  spdlog::info("[{}] connected", description);

  return true;
}

void Handler::handleClientDisconnect() {
  auto session = qobject_cast<Session *>(sender());
  auto description = session->getDescription();

  spdlog::info("[{}] disconnected", description);

  if (session->getRole() == Bridge::Role::controller) {
    // Check if this is not a duplicated connection
    if (getClient(session->getHwid()) == session) {
      clients.erase(session->getHwid());
      emit clientDisconnected(session);
    }

    session->deleteLater();
  } else if (session->getRole() == Bridge::Role::unknown) {
    auto iter = std::find(pending_queue.begin(), pending_queue.end(), session);
    pending_queue.erase(iter);

    session->deleteLater();
  }
}
} // namespace Network