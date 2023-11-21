#include "Controller.hpp"
#include "Handshake.hpp"
#include "HeartbeatChannel.hpp"
#include "Util/Util.hpp"
#include <magic_enum.hpp>
#include <spdlog/spdlog.h>

namespace Network {
void Controller::run() {
  performHandshake(Bridge::Role::controller);

  REGISTER_CALLBACK(ServerHandshake, Controller);

  while (true) {
    auto parser = readJsonPacket();

    if (parser.has_value() == false) {
      return;
    }

    dispatch(parser.value());
  }
}

bool Controller::checkSubChannelExist(Bridge::Role role) {
  try {
    auto previous = sub_channels.at(role);
    return true;
  } catch (const std::exception &) {
  }

  return false;
}

bool Controller::createSubChannel(const Bridge::Role role, const uint64_t id) {
  if (checkSubChannelExist(role) == true) {
    return false;
  }

  AbstractClient *client = nullptr;

  if (role == Bridge::Role::heartbeat) {
    client = new HeartbeatChannel(io_context, getEndpoint(), id);
  } else {
    spdlog::error("role not unhandled, role: {}", magic_enum::enum_name(role));
    assert(false);
    return false;
  }

  if (client->connect() == false) {
    delete client;
    return false;
  }

  auto thread = new std::thread(&AbstractClient::run, client);
  sub_channels[role] = std::make_tuple(thread, client);
  return true;
}

Controller::~Controller() {
  for (auto &channel_pool : sub_channels) {
    auto thread = std::get<0>(channel_pool.second);
    auto channel = std::get<1>(channel_pool.second);

    channel->shutdown();

    if (thread->joinable()) {
      thread->join();
    }

    delete channel;
    delete thread;
  }
}

bool Controller::onServerHandshake(const Bridge::Parser &parser) {
  GET_BODY(ServerHandshake);

  spdlog::info("connected, message: {}", body->message);

  auto ret = sendJsonPacket(
      GENERATE_PACKET(Bridge::ClientInformation, Util::getClientInformation()));

  if (ret != true) {
    spdlog::error("can not reply server handshake");
    return false;
  }

  return createSubChannel(Bridge::Role::heartbeat, 0);
}
} // namespace Network
