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

  const auto type = header->type;

  invoke(type, parser);
}

bool AbstractClient::invoke(Bridge::Type type,
                            const Bridge::Parser &parser) const {
  try {
    auto &signal = callbacks.at(type);
    return signal.emit(parser);
  } catch (const std::exception &) {
    spdlog::error("packet not handled, type: {}", magic_enum::enum_name(type));
    return false;
  }
}
} // namespace Network
