#include "Heartbeat.h"
#include "AApch.h"

std::string Packet::RequestHeartbeatChannel::buildJson() const {
  nlohmann::json packet;

  return packet.dump(-1, ' ', true);
}

void Packet::RequestHeartbeatChannel::parseJson(const std::string &buffer) {
  const nlohmann::json packet = nlohmann::json::parse(buffer);
}

std::string Packet::ResponseHeartbeat::buildJson() const {
  nlohmann::json packet;

  packet["focus_window"] = focus_window;

  return packet.dump(-1, ' ', true);
}

void Packet::ResponseHeartbeat::parseJson(const std::string &buffer) {
  const nlohmann::json packet = nlohmann::json::parse(buffer);

  focus_window = packet["focus_window"];
}