#include "ExecuteFile.h"
#include "AApch.h"

std::string Packet::RequestExecuteFile::buildJson() const {
  nlohmann::json packet;

  EMPLACE_PARAM(path);

  return packet.dump(-1, ' ', true);
}

void Packet::RequestExecuteFile::parseJson(const std::string &buffer) {
  const nlohmann::json packet = nlohmann::json::parse(buffer);

  path = packet["path"];
}

std::string Packet::ResponseExecuteFile::buildJson() const {
  nlohmann::json packet;

  packet["error_code"] = error_code;

  return packet.dump(-1, ' ', true);
}

void Packet::ResponseExecuteFile::parseJson(const std::string &buffer) {
  const nlohmann::json packet = nlohmann::json::parse(buffer);

  error_code = packet["error_code"];
}