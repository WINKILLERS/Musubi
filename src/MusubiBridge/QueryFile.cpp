#include "QueryFile.h"
#include "AApch.h"

std::string Packet::RequestFileChannel::buildJson() const {
  nlohmann::json packet;

  return packet.dump(-1, ' ', true);
}

void Packet::RequestFileChannel::parseJson(const std::string &buffer) {
  const nlohmann::json packet = nlohmann::json::parse(buffer);
}

std::string Packet::RequestQueryFile::buildJson() const {
  nlohmann::json packet;

  EMPLACE_PARAM(path);

  return packet.dump(-1, ' ', true);
}

void Packet::RequestQueryFile::parseJson(const std::string &buffer) {
  const nlohmann::json packet = nlohmann::json::parse(buffer);

  EXTRACT_PARAM(path);
}

std::string Packet::ResponseQueryFile::buildJson() const {
  nlohmann::json packet;

  EMPLACE_PARAM(is_error);

  auto &node = packet["list"];
  for (const auto &f : list) {
    nlohmann::json sub;
    sub["name"] = f.name;
    sub["is_directory"] = f.is_directory;
    node.push_back(std::move(sub));
  }

  return packet.dump(-1, ' ', true);
}

void Packet::ResponseQueryFile::parseJson(const std::string &buffer) {
  const nlohmann::json packet = nlohmann::json::parse(buffer);

  EXTRACT_PARAM(is_error);

  auto &node = packet["list"];
  for (const auto &sub : node) {
    File f;
    f.name = sub["name"];
    f.is_directory = sub["is_directory"];
    list.push_back(std::move(f));
  }
}