#include "Header.h"
#include "AApch.h"

std::string Packet::Header::buildJson() const {
  nlohmann::json packet;

  EMPLACE_PARAM(type);
  EMPLACE_PARAM(id);
  EMPLACE_PARAM(timestamp);
  EMPLACE_PARAM(version);

  return packet.dump(-1, ' ', true);
}

void Packet::Header::parseJson(const std::string &buffer) {
  const nlohmann::json packet = nlohmann::json::parse(buffer);

  EXTRACT_PARAM(type);
  EXTRACT_PARAM(id);
  EXTRACT_PARAM(timestamp);
  EXTRACT_PARAM(version);
}