#include "Controller.hpp"
#include "Handshake.hpp"
#include "HeartbeatChannel.hpp"
#include "TerminateProcess.hpp"
#include "Util/Util.hpp"
#include <magic_enum.hpp>
#include <spdlog/spdlog.h>
#include <utf8.h>

namespace Network {
void Controller::run() {
  performHandshake(Bridge::Role::controller);

  REGISTER_CALLBACK(ServerHandshake, Controller);
  REGISTER_CALLBACK(RequestGetProcesses, Controller);
  REGISTER_CALLBACK(RequestTerminateProcess, Controller);

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

  if (body->shutdown) {
    spdlog::info("shutdown by server");
    shutdown_by_server = true;
    shutdown();
    return false;
  }

  auto ret = sendJsonPacket(
      GENERATE_PACKET(Bridge::ClientInformation, Util::getInformation()));

  if (ret != true) {
    spdlog::error("can not reply server handshake");
    return false;
  }

  // No longer need heartbeat
  return createSubChannel(Bridge::Role::heartbeat, 0);
  return true;
}

bool Controller::onRequestGetProcesses(const Bridge::Parser &parser) {
  auto opt_response = Util::getProcesses();

  if (opt_response.has_value() == false) {
    spdlog::error("can not get processes");
    return false;
  }

  auto response = opt_response.value();

  auto ret =
      sendJsonPacket(GENERATE_PACKET(Bridge::ResponseGetProcesses, response));

  if (ret != true) {
    spdlog::error("can not reply get processes");
    return false;
  }

  return true;
}

bool Controller::onRequestTerminateProcess(const Bridge::Parser &parser) {
  GET_BODY(RequestTerminateProcess);

  auto opt_response = Util::terminateProcess(body);

  if (opt_response.has_value() == false) {
    spdlog::error("can not terminate process");
    return false;
  }

  auto response = opt_response.value();

  auto ret = sendJsonPacket(
      GENERATE_PACKET(Bridge::ResponseTerminateProcess, response));

  if (ret != true) {
    spdlog::error("can not reply terminate process");
    return false;
  }

  return true;
}
} // namespace Network
