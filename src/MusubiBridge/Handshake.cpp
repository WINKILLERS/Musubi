#include "Handshake.hpp"
#include <nlohmann/json.hpp>

namespace Bridge {
std::string ClientInformation::buildJson() const {
  nlohmann::json packet;

  EMPLACE_PARAM(cpu_model);
  EMPLACE_PARAM(os_name);
  EMPLACE_PARAM(user_name);
  EMPLACE_PARAM(computer_name);

  return packet.dump(-1, ' ', true);
}

void ClientInformation::parseJson(const std::string &json) {
  const nlohmann::json packet = nlohmann::json::parse(json);

  EXTRACT_PARAM(cpu_model);
  EXTRACT_PARAM(os_name);
  EXTRACT_PARAM(user_name);
  EXTRACT_PARAM(computer_name);
}

std::string ClientHandshake::buildJson() const {
  nlohmann::json packet;

  EMPLACE_PARAM(hwid);
  EMPLACE_PARAM(role);

  return packet.dump(-1, ' ', true);
}

void ClientHandshake::parseJson(const std::string &json) {
  const nlohmann::json packet = nlohmann::json::parse(json);

  EXTRACT_PARAM(hwid);
  EXTRACT_PARAM(role);
}

std::string ServerHandshake::buildJson() const {
  nlohmann::json packet;

  EMPLACE_PARAM(message);

  return packet.dump(-1, ' ', true);
}

void ServerHandshake::parseJson(const std::string &json) {
  const nlohmann::json packet = nlohmann::json::parse(json);

  EXTRACT_PARAM(message);
}
} // namespace Bridge