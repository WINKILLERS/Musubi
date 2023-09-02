#include "AutoStart.h"
#include "AApch.h"

std::string Packet::RequestTaskAutoStart::buildJson() const {
  nlohmann::json packet;

  packet["type"] = type;

  return packet.dump(-1, ' ', true);
}

void Packet::RequestTaskAutoStart::parseJson(const std::string &buffer) {
  const nlohmann::json packet = nlohmann::json::parse(buffer);

  type = packet["type"];
}

std::string Packet::ResponseTaskAutoStart::buildJson() const {
  nlohmann::json packet;

  packet["error_code"] = error_code;

  return packet.dump(-1, ' ', true);
}

void Packet::ResponseTaskAutoStart::parseJson(const std::string &buffer) {
  const nlohmann::json packet = nlohmann::json::parse(buffer);

  error_code = packet["error_code"];
}