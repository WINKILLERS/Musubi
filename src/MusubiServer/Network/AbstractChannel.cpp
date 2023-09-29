#include "AbstractChannel.h"
#include "AApch.h"
#include "AbstractProtocol.h"
#include "FileChannel.h"
#include "HeartbeatChannel.h"
#include "InputChannel.h"
#include "ProgramChannel.h"
#include "ScreenChannel.h"
#include "qeventloop.h"

Network::AbstractChannel::AbstractChannel(AbstractSession *session,
                                          AbstractChannel *parent)
    : session(session), parent(parent), QObject(parent) {
  connect(&session->notifier, &IPacketNotify::lag, this, &AbstractChannel::lag);
}

Network::AbstractChannel::~AbstractChannel() {
  if (window != nullptr) {
    window->close();
    window->deleteLater();
    window = nullptr;
  }

  // Delete from memory
  session->deleteSelf();
  session = nullptr;

  parent = nullptr;
}

bool Network::AbstractChannel::showWindow(QWidget *parent) {
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
    spdlog::debug("deleted subchannel: {}",
                  magic_enum::enum_name(sub_channel.first));

    sub_channel.second->deleteLater();
  }

  sub_channels.clear();

  pending_tasks.clear();
}

Network::AbstractChannel *Network::AbstractMultiChannel::getSubChannel(
    Packet::Handshake::Role role) const {
  try {
    return sub_channels.at(role);
  } catch (const std::exception &) {
    return nullptr;
  }
}

std::optional<QFuture<Network::AbstractChannel *>>
Network::AbstractMultiChannel::requestOpenChannel(
    Packet::Handshake::Role role) {
  auto sub_channel = getSubChannel(role);
  if (sub_channel != nullptr) {
    spdlog::warn("{} subchannel already exist", magic_enum::enum_name(role));
    QPromise<AbstractChannel *> promise;
    promise.addResult(sub_channel);
    promise.finish();
    return promise.future();
  }

  Packet::AbstractGenerator *generator;

  if (role == Packet::Handshake::Role::file) {
    generator = new Packet::Generator<Packet::RequestFileChannel>();
  } else if (role == Packet::Handshake::Role::keyboard_monitor) {
    generator = new Packet::Generator<Packet::RequestKeyboardMonitorChannel>();
  } else if (role == Packet::Handshake::Role::remote_hidden_screen) {
    generator = new Packet::Generator<Packet::RequestScreenChannel>(true);
  } else if (role == Packet::Handshake::Role::remote_screen) {
    generator = new Packet::Generator<Packet::RequestScreenChannel>(false);
  } else if (role == Packet::Handshake::Role::heartbeat) {
    generator = new Packet::Generator<Packet::RequestHeartbeatChannel>();
  } else if (role == Packet::Handshake::Role::program) {
    generator = new Packet::Generator<Packet::RequestProgramChannel>();
  } else if (role == Packet::Handshake::Role::input) {
    generator = new Packet::Generator<Packet::RequestInputChannel>();
  } else {
    spdlog::error("bug detected, unhandled open request");
    assert(false);
    return std::nullopt;
  }

  auto id = generator->getId();
  auto send_ret = session->sendJsonPacket(*generator);
  delete generator;

  if (send_ret.has_value() == false) {
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

  if (role == Packet::Handshake::Role::file) {
    channel = new FileChannel(sub_session, this);
    //} else if (role == Packet::Handshake::Role::keyboard_monitor) {
    //} else if (role == Packet::Handshake::Role::remote_hidden_screen) {
  } else if (role == Packet::Handshake::Role::remote_screen) {
    channel = new ScreenChannel(sub_session, this);
  } else if (role == Packet::Handshake::Role::heartbeat) {
    channel = new HeartbeatChannel(sub_session, this);
  } else if (role == Packet::Handshake::Role::program) {
    channel = new ProgramChannel(sub_session, this);
  } else if (role == Packet::Handshake::Role::input) {
    channel = new InputChannel(sub_session, this);
  } else {
    spdlog::error("bug detected, unhandled subchannel");
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

void Network::AbstractMultiChannel::removeSubChannel(AbstractSession *session) {
  auto sub_channel = getSubChannel(session->getRole());
  sub_channel->deleteLater();

  sub_channels.erase(session->getRole());
}