#include "DeleteFile.h"
#include "AApch.h"

std::string Packet::RequestDeleteFile::buildJson() const {
  nlohmann::json packet;

  EMPLACE_PARAM(path);

  return packet.dump(-1, ' ', true);
}

void Packet::RequestDeleteFile::parseJson(const std::string &buffer) {
  const nlohmann::json packet = nlohmann::json::parse(buffer);

  EXTRACT_PARAM(path);
}

std::string Packet::ResponseDeleteFile::buildJson() const {
  nlohmann::json packet;

  EMPLACE_PARAM(removed_count);

  return packet.dump(-1, ' ', true);
}

void Packet::ResponseDeleteFile::parseJson(const std::string &buffer) {
  const nlohmann::json packet = nlohmann::json::parse(buffer);

  EXTRACT_PARAM(removed_count);
}