#include "Handshake.hpp"
#include <nlohmann/json.hpp>

namespace Bridge {
std::string ClientHandshake::buildJson() const {
  nlohmann::json packet;

  EMPLACE_PARAM(hwid);

  return packet.dump(-1, ' ', true);
}

void ClientHandshake::parseJson(const std::string &json) {
  const nlohmann::json packet = nlohmann::json::parse(json);

  EXTRACT_PARAM(hwid);
}
} // namespace Bridge