#include "Controller.hpp"
#include <magic_enum.hpp>
#include <spdlog/spdlog.h>

void Network::Controller::run() {
  performHandshake(Bridge::Role::controller);

  while (true) {
    auto parser = readJsonPacket();

    if (parser.has_value() == false) {
      return;
    }

    dispatch(parser.value());
  }
}

bool Network::Controller::checkSubChannelExist(Bridge::Role role) {
  try {
    auto previous = sub_channels.at(role);
    return true;
  } catch (const std::exception &) {
  }

  return false;
}

bool Network::Controller::createSubChannel(Bridge::Role role,
                                           const std::string &id) {
  if (checkSubChannelExist(role) == true) {
    return false;
  }

  AbstractClient *client = nullptr;

  if (false) {
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

Network::Controller::~Controller() {
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