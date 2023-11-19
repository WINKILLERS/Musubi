#include "AbstractHandler.hpp"
#include "../Controller/Controller.hpp"
#include "AbstractSession.hpp"
#include <spdlog/spdlog.h>
#include <utility>

namespace Network {
void ClientNotifier::emitClientConnected(const Controller *controller) {
  emit clientConnected(controller);
}

void ClientNotifier::emitClientDisconnected(const Controller *controller) {
  emit clientDisconnected(controller);
}

Controller *AbstractHandler::getClient(const std::string &hwid) const {
  try {
    return clients.at(hwid);
  } catch (const std::exception &) {
    // The controller is deleted, we can not find
    return nullptr;
  }
}

Controller *AbstractHandler::getClient(const AbstractSession *session) const {
  return getClient(session->getHwid());
}

bool AbstractHandler::addPendingSession(AbstractSession *session) {
  const auto description = session->getDescription();

  try {
    pending_queue.push_back(session);

    spdlog::debug(
        "[{}] new session established connection, current pending count: {}",
        description, getPendingCount());
  } catch (const std::exception &) {
    spdlog::error("[{}] can not add pending session", description);
    return false;
  }

  return true;
}

bool AbstractHandler::removeSession(const AbstractSession *session) {
  const auto description = session->getDescription();

  // If the session is a controller
  if (session->getRole() == Bridge::Role::controller) {
    spdlog::info("[{}] disconnected", description);

    auto controller = getClient(session);
    if (controller == nullptr) {
      spdlog::error(
          "[{}] bug detected, trying to remove a non-exist controller",
          description);
      assert(false);
      return false;
    }

    // Delete from clients
    clients.erase(session->getHwid());

    // Notify disconnection
    notifier.emitClientDisconnected(controller);

    // Delete from memory
    controller->deleteLater();
  }
  // If the session is owned by a controller
  else if (session->getRole() != Bridge::Role::unknown) {
    auto controller = getClient(session);

    // Delete from controller
    controller->removeSubChannel(session);
  }
  // If the session is in pending queue
  else {
    // Find the session in queue
    auto iter = std::find(pending_queue.begin(), pending_queue.end(), session);
    if (iter == pending_queue.cend()) {
      spdlog::error("[{}] bug detected, trying to remove a non-exist session",
                    description);
      assert(false);
      return false;
    }

    // Delete from memory
    (*iter)->deleteSelf();

    // Remove from queue
    pending_queue.erase(iter);
  }

  return true;
}

bool AbstractHandler::migratePendingSession(const AbstractSession *session) {
  const auto description = session->getDescription();

  // Basic check
  if (session->getRole() == Bridge::Role::unknown) {
    spdlog::error(
        "[{}] bug detected, trying to migrate a uninitialized session",
        description);
    assert(false);
    return false;
  }

  // Find the session in queue
  auto iter = std::find(pending_queue.cbegin(), pending_queue.cend(), session);
  if (iter == pending_queue.cend()) {
    spdlog::error("[{}] bug detected, trying to migrate a non-exist session",
                  description);
    assert(false);
    return false;
  }

  // Remove the session from pending queue
  pending_queue.erase(iter);

  // Output new connection
  spdlog::info("[{}] connected", description);

  // If role is controller
  if (session->getRole() == Bridge::Role::controller) {
    // Construct controller
    auto controller = new Controller(session);

    // Add the session to controller
    clients[session->getHwid()] = controller;

    // Notify new controller
    notifier.emitClientConnected(controller);
  }
  // Or we are a channel session
  else {
    // Get the hwid for the session and get controller
    auto controller = getClient(session->getHwid());

    // Add the sub channel to controller
    controller->addSubChannel(session);
  }

  return true;
}
} // namespace Network