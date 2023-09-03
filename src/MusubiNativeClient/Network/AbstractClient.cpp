#include "AbstractClient.h"
#include "AApch.h"

Network::AbstractClient::AbstractClient(const std::string &handshake_id)
    : handshake_id(handshake_id) {
  HW_PROFILE_INFOA hw_profile;
  if (GetCurrentHwProfileA(&hw_profile))
    hwid = hw_profile.szHwProfileGuid;

  spdlog::debug("session ctor, current hwid: {}", fmt::ptr(this), hwid);
}

Network::AbstractClient::~AbstractClient() {
  spdlog::info("session {} dtor", fmt::ptr(this));
}

bool Network::AbstractClient::performHandshake(
    Packet::Handshake::Role role) noexcept {
  spdlog::debug("performing handshake with role: {}, id: {}",
                magic_enum::enum_name(role), handshake_id);
  auto generator = Packet::Generator<Packet::Handshake>(hwid, role);
  generator.setId(handshake_id);

  return sendJsonPacket(generator);
}

void Network::AbstractClient::dispatch(const Packet::Parser &parser) noexcept {
  spdlog::debug("received packet with type: {}",
                magic_enum::enum_name(parser.header->type));

  try {
    invoke(parser.header->type, parser.header, parser.body);
  } catch (const std::exception &) {
  }
}

bool Network::AbstractClient::invoke(
    Packet::Type type, std::shared_ptr<Packet::Header> header,
    std::shared_ptr<Packet::AbstractPacket> packet) const noexcept {
  try {
    auto &signal = callbacks.at(type);
    return signal.emit(header, packet);
  } catch (const std::exception &) {
    assert(false);
    return false;
  }
}