#include "TerminateProcess.h"
#include "AApch.h"

std::string Packet::RequestTerminateProcess::buildJson() const {
  nlohmann::json packet;

  packet["pid"] = pid;

  return packet.dump(-1, ' ', true);
}

void Packet::RequestTerminateProcess::parseJson(const std::string &buffer) {
  const nlohmann::json packet = nlohmann::json::parse(buffer);

  pid = packet["pid"];
}

std::string Packet::ResponseTerminateProcess::buildJson() const {
  nlohmann::json packet;

  packet["error_code"] = error_code;

  return packet.dump(-1, ' ', true);
}

void Packet::ResponseTerminateProcess::parseJson(const std::string &buffer) {
  const nlohmann::json packet = nlohmann::json::parse(buffer);

  error_code = packet["error_code"];
}