#include "AbstractChannel.hpp"
#include <spdlog/spdlog.h>

namespace Network {
AbstractChannel::AbstractChannel(AbstractSession *session,
                                 AbstractChannel *parent)
    : session(session), parent(parent), QObject(parent) {}

Network::AbstractChannel::~AbstractChannel() {
  if (window != nullptr) {
    window->deleteLater();
  }

  // Delete from memory
  session->deleteSelf();
}

bool Network::AbstractChannel::showWindow() {
  // If we have no window
  if (window == nullptr) {
    return false;
  }

  // Or we have one, just show it
  window->show();
  return true;
}

Network::AbstractMultiChannel::~AbstractMultiChannel() {
  for (auto &sub_channel : sub_channels) {
    sub_channel.second->deleteLater();
  }

  sub_channels.clear();

  pending_tasks.clear();
}

Network::AbstractChannel *
Network::AbstractMultiChannel::getSubChannel(Bridge::Role role) const {
  try {
    return sub_channels.at(role);
  } catch (const std::exception &) {
    return nullptr;
  }
}

std::optional<QFuture<Network::AbstractChannel *>>
Network::AbstractMultiChannel::requestOpenChannel(Bridge::Role role) {
  auto sub_channel = getSubChannel(role);
  if (sub_channel != nullptr) {
    QPromise<AbstractChannel *> promise;
    promise.addResult(sub_channel);
    promise.finish();
    return promise.future();
  }

  Bridge::AbstractGenerator *gen;

  if (false) {
  } else {
    spdlog::error("bug detected, unhandled open request");
    assert(false);
    return std::nullopt;
  }

  const auto id = (uint64_t)gen;
  gen->setId(id);
  auto send_ret = session->sendJsonPacket(*gen);
  delete gen;

  if (send_ret == false) {
    spdlog::error("can not send packet");
    return std::nullopt;
  }

  auto &task = pending_tasks[id];

  return task.future();
}

bool Network::AbstractMultiChannel::addSubChannel(
    AbstractSession *sub_session) {
  auto role = sub_session->getRole();
  AbstractChannel *channel = nullptr;

  if (false) {
  } else {
    spdlog::error("bug detected, unhandled sub channel");
    assert(false);
    return false;
  }

  sub_channels[sub_session->getRole()] = channel;
  emit channelConnected(role, channel);

  try {
    auto id = sub_session->getHandshakeId();
    auto task = std::move(pending_tasks.at(id));
    pending_tasks.erase(id);
    task.addResult(channel);
    task.finish();
  } catch (const std::exception &) {
  }

  return true;
}

void AbstractMultiChannel::removeSubChannel(const AbstractSession *session) {
  auto sub_channel = getSubChannel(session->getRole());
  if (sub_channel != nullptr) {
    sub_channel->deleteLater();

    sub_channels.erase(session->getRole());
  }
}
} // namespace Network
