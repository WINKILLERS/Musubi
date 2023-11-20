#include "AbstractSession.hpp"
#include "AbstractHandler.hpp"
#include "Factory.hpp"
#include "Handshake.hpp"
#include "spdlog/fmt/bundled/core.h"
#include <magic_enum.hpp>
#include <spdlog/spdlog.h>

namespace Network {
bool PacketNotifier::dispatch(const Bridge::Parser &parser) {
  // Get packet's information
  const auto &header = parser.getHeader();

  // Get header fields
  auto type = header->type;

  switch (type) {
  default:
    spdlog::error("packet not handled, type: {}", magic_enum::enum_name(type));
    return false;
  };

  return true;
}

AbstractSession::AbstractSession(AbstractHandler *handler_)
    : handler(handler_) {}

AbstractSession::~AbstractSession() {}

std::string AbstractSession::getDescription() const {
  return fmt::format("{}://{}: {}", magic_enum::enum_name(getType()),
                     getRemoteAddress(), magic_enum::enum_name(role));
}

bool AbstractSession::processPacket(const std::string &buffer) {
  const auto description = getDescription();

  // Parse the request
  Bridge::Parser parser;
  if (parser.parseJson(buffer) == false) {
    spdlog::warn("[{}] error when parsing packet", description);
    return false;
  }

  // Get packet's information
  const auto &header = parser.getHeader();

  // Get header fields
  const auto type = header->type;
  const auto id = header->id;
  const auto timestamp = header->timestamp;

  // Check is client sending server command
  if (PACKET_SERVER_TYPE(type) == true) {
    spdlog::error("[{}] session sended server command", description);
    shutdown();
    return false;
  }

  // Process handshake
  if (type == Bridge::Type::client_handshake) {
    const auto &body = parser.getBody<Bridge::ClientHandshake>();
    hwid = body->hwid;
    role = body->role;

    // If hwid not present, the client is invalid
    if (hwid.empty()) {
      spdlog::error("[{}] session hwid empty", description);
      shutdown();
      return false;
    }

    // If role not set, the client is invalid
    if (role == Bridge::Role::unknown) {
      spdlog::error("[{}] session role error", description);
      shutdown();
      return false;
    }

    // Server internal error
    auto migrate = handler->migratePendingSession(this);
    if (migrate != true) {
      spdlog::error("[{}] session can not migrate", description);
      shutdown();
      return false;
    }

    handshake_id = id;

    return true;
  }

  // If we have not performed handshake
  if (role != Bridge::Role::unknown) {
    spdlog::error("[{}] session sending request before initialization",
                  description);
    shutdown();
    return false;
  }

  return notifier.dispatch(parser);
}
} // namespace Network
