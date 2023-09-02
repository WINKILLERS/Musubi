#include "GetInformation.h"
#include "AApch.h"

std::string Packet::RequestInformation::buildJson() const {
  nlohmann::json packet;

  return packet.dump(-1, ' ', true);
}

void Packet::RequestInformation::parseJson(const std::string &buffer) {
  const nlohmann::json packet = nlohmann::json::parse(buffer);
}

std::string Packet::ResponseInformation::buildJson() const {
  nlohmann::json packet;

  packet["cpu_model"] = cpu_model;
  packet["os_name"] = os_name;
  packet["user_name"] = user_name;
  packet["computer_name"] = computer_name;

  return packet.dump(-1, ' ', true);
}

void Packet::ResponseInformation::parseJson(const std::string &buffer) {
  const nlohmann::json packet = nlohmann::json::parse(buffer);

  cpu_model = packet["cpu_model"];
  os_name = packet["os_name"];
  user_name = packet["user_name"];
  computer_name = packet["computer_name"];
}