#include "AbstractPacket.h"
#include "AApch.h"

constexpr const uint32_t BRIDGE_VERSION = MAGIC;

uint32_t getBridgeVersion() { return BRIDGE_VERSION; }

std::string Packet::AbstractPacket::buildJson() const {
  nlohmann::json packet;

  return packet.dump(-1, ' ', true);
}

void Packet::AbstractPacket::parseJson(const std::string &buffer) {
  const nlohmann::json packet = nlohmann::json::parse(buffer);
}