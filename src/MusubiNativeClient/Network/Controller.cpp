#include "Controller.h"
#include "AApch.h"
#include "File.h"
#include "Heartbeat.h"
#include "Program.h"
#include "Screen.h"
#include "Util/Util.h"


void Network::Controller::run() noexcept {
  performHandshake();

  registerCallback(Packet::Type::request_information, *this,
                   &Network::Controller::onInformation);
  registerCallback(Packet::Type::request_get_process, *this,
                   &Network::Controller::onGetProcess);
  registerCallback(Packet::Type::request_heartbeat_channel, *this,
                   &Network::Controller::onHeartbeatChannel);
  registerCallback(Packet::Type::request_file_channel, *this,
                   &Network::Controller::onFileChannel);
  registerCallback(Packet::Type::request_program_channel, *this,
                   &Network::Controller::onProgramChannel);
  registerCallback(Packet::Type::request_screen_channel, *this,
                   &Network::Controller::onScreenChannel);

  while (true) {
    auto parser = readJsonPacket();

    if (parser.has_value() == false) {
      return;
    }

    dispatch(parser.value());
  }
}

bool Network::Controller::onInformation(
    std::shared_ptr<Packet::Header> header,
    std::shared_ptr<Packet::AbstractPacket> param) noexcept {
  return sendJsonPacket(
      Packet::Generator<Packet::ResponseInformation>(getInformation()));
}

bool Network::Controller::onGetProcess(
    std::shared_ptr<Packet::Header> header,
    std::shared_ptr<Packet::AbstractPacket> param) noexcept {
  return sendJsonPacket(
      Packet::Generator<Packet::ResponseGetProcess>(getProcess()));
}

bool Network::Controller::onHeartbeatChannel(
    std::shared_ptr<Packet::Header> header,
    std::shared_ptr<Packet::AbstractPacket> param) noexcept {
  return createSubchannel(Packet::Handshake::Role::heartbeat, header->id);
}

bool Network::Controller::onFileChannel(
    std::shared_ptr<Packet::Header> header,
    std::shared_ptr<Packet::AbstractPacket> param) noexcept {
  return createSubchannel(Packet::Handshake::Role::file, header->id);
}

bool Network::Controller::onProgramChannel(
    std::shared_ptr<Packet::Header> header,
    std::shared_ptr<Packet::AbstractPacket> param) noexcept {
  return createSubchannel(Packet::Handshake::Role::program, header->id);
}

bool Network::Controller::onScreenChannel(
    std::shared_ptr<Packet::Header> header,
    std::shared_ptr<Packet::AbstractPacket> param) noexcept {
  return createSubchannel(Packet::Handshake::Role::remote_screen, header->id);
}

bool Network::Controller::checkSubchannelExist(
    Packet::Handshake::Role role) noexcept {
  try {
    auto previous = sub_channels.at(role);
    spdlog::warn("previous role already exist: {}",
                 magic_enum::enum_name(role));
    return true;
  } catch (const std::exception &) {
  }

  return false;
}

bool Network::Controller::createSubchannel(Packet::Handshake::Role role,
                                           const std::string &id) noexcept {
  if (checkSubchannelExist(role) == true) {
    return false;
  }

  AbstractClient *client = nullptr;

  if (role == Packet::Handshake::Role::heartbeat) {
    client = new Heartbeat(io_context, endpoint, id);
  } else if (role == Packet::Handshake::Role::file) {
    client = new File(io_context, endpoint, id);
  } else if (role == Packet::Handshake::Role::program) {
    client = new Program(io_context, endpoint, id);
  } else if (role == Packet::Handshake::Role::remote_screen) {
    client = new Screen(io_context, endpoint, id);
  } else {
    spdlog::error("bug detected, unhandled role");
    assert(false);
    return false;
  }

  if (client->connect() == false) {
    spdlog::error("{} can not connect", magic_enum::enum_name(role));
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