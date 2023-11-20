#include "AbstractClient.hpp"
#include "Handshake.hpp"
#include <magic_enum.hpp>
#include <spdlog/spdlog.h>

namespace Network {
AbstractClient::AbstractClient(const uint64_t handshake_id)
    : handshake_id(handshake_id) {
  HW_PROFILE_INFOA hw_profile;
  if (GetCurrentHwProfileA(&hw_profile)) {
    hwid = hw_profile.szHwProfileGuid;
  }
}

AbstractClient::~AbstractClient() {}

bool AbstractClient::performHandshake(Bridge::Role role) {
  spdlog::debug("performing handshake with role: {}, id: {}",
                magic_enum::enum_name(role), handshake_id);

  return sendJsonPacket(GENERATE_PACKET_WITH_ID(
      Bridge::ClientHandshake, handshake_id, hwid, Bridge::Role::controller));
}

void AbstractClient::dispatch(const Bridge::Parser &parser) {
  const auto header = parser.getHeader();
  const auto body = parser.getBody();

  const auto type = header->type;

  spdlog::debug("received packet with type: {}", magic_enum::enum_name(type));

  invoke(type, header, body);
}

bool AbstractClient::invoke(Bridge::Type type, Bridge::HeaderPtr header,
                            Bridge::BodyPtr packet) const {
  try {
    auto &signal = callbacks.at(type);
    return signal.emit(header, packet);
  } catch (const std::exception &) {
    assert(false);
    return false;
  }
}
} // namespace Network
