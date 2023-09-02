#include "StartProcess.h"
#include "AApch.h"

std::string Packet::RequestStartProcess::buildJson() const {
  nlohmann::json packet;

  EMPLACE_PARAM(path);

  return packet.dump(-1, ' ', true);
}

void Packet::RequestStartProcess::parseJson(const std::string &buffer) {
  const nlohmann::json packet = nlohmann::json::parse(buffer);

  path = packet["path"];
}

std::string Packet::ResponseStartProcess::buildJson() const {
  nlohmann::json packet;

  packet["error_code"] = error_code;

  return packet.dump(-1, ' ', true);
}

void Packet::ResponseStartProcess::parseJson(const std::string &buffer) {
  const nlohmann::json packet = nlohmann::json::parse(buffer);

  error_code = packet["error_code"];
}