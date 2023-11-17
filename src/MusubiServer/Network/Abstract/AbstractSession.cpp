#include "AbstractSession.hpp"
#include "Factory.hpp"
#include <magic_enum.hpp>
#include <spdlog/spdlog.h>

bool Network::PacketNotifier::dispatch(const Bridge::Parser &parser) {
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

bool Network::AbstractSession::processPacket(const std::string &buffer) {
  const auto remote_address = getRemoteAddress();

  // Parse the request
  Bridge::Parser parser;
  if (parser.parseJson(buffer) == false) {
    spdlog::warn("[{}] error when parsing packet", remote_address);
    return false;
  }

  // Get packet's information
  const auto &header = parser.getHeader();

  // Get header fields
  auto type = header->type;
  const auto &id = header->id;
  auto timestamp = header->timestamp;

  // Check is client sending server command
  if (SERVER_TYPE(type) == true) {
    spdlog::error("[{}] session sended server command", remote_address);
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
      spdlog::error("[{}] session hwid empty", remote_address);
      shutdown();
      return false;
    }

    // If role not set, the client is invalid
    if (role == Bridge::Role::unknown) {
      spdlog::error("[{}] session role error", remote_address);
      shutdown();
      return false;
    }

    return true;
  }

  // If we have not performed handshake
  if (role != Bridge::Role::unknown) {
    spdlog::error("[{}] session sending request before initialization",
                  remote_address);
    shutdown();
    return false;
  }

  return true;
}