#include "Packet.hpp"
#include <chrono>
#include <nlohmann/json.hpp>

namespace Bridge {
constexpr const uint32_t BRIDGE_VERSION = BUILD_MAGIC;

uint64_t getBridgeVersion() { return BRIDGE_VERSION; }

std::string AbstractPacket::buildJson() const {
  nlohmann::json packet;

  return packet.dump(-1, ' ', true);
}

void AbstractPacket::parseJson(const std::string &json) {
  const nlohmann::json packet = nlohmann::json::parse(json);
}

std::string Header::buildJson() const {
  nlohmann::json packet;

  EMPLACE_PARAM(type);
  EMPLACE_PARAM(id);
  EMPLACE_PARAM(timestamp);
  EMPLACE_PARAM(version);

  return packet.dump(-1, ' ', true);
}

void Header::parseJson(const std::string &json) {
  const nlohmann::json packet = nlohmann::json::parse(json);

  EXTRACT_PARAM(type);
  EXTRACT_PARAM(id);
  EXTRACT_PARAM(timestamp);
  EXTRACT_PARAM(version);
}

void Header::initializeTimestamp() {
  timestamp = std::chrono::duration_cast<std::chrono::milliseconds>(
                  std::chrono::system_clock::now().time_since_epoch())
                  .count();
}
} // namespace Bridge