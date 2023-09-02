#include "KeyboardMonitor.h"
#include "AApch.h"

std::string Packet::RequestKeyboardMonitorChannel::buildJson() const {
  nlohmann::json packet;

  return packet.dump(-1, ' ', true);
}

void Packet::RequestKeyboardMonitorChannel::parseJson(
    const std::string &buffer) {
  const nlohmann::json packet = nlohmann::json::parse(buffer);
}

std::string Packet::ResponseGetKey::buildJson() const {
  nlohmann::json packet;

  packet["action"] = input.action;
  packet["keys"] = input.keys;
  packet["virtual_code"] = input.virtual_code;

  return packet.dump(-1, ' ', true);
}

void Packet::ResponseGetKey::parseJson(const std::string &buffer) {
  const nlohmann::json packet = nlohmann::json::parse(buffer);

  auto &node = packet["keyboard"];

  input.action = packet["action"];
  input.keys = packet["keys"];
  input.virtual_code = packet["virtual_code"];
}