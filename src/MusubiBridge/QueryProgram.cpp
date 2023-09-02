#include "QueryProgram.h"
#include "AApch.h"

std::string Packet::RequestProgramChannel::buildJson() const {
  nlohmann::json packet;

  return packet.dump(-1, ' ', true);
}

void Packet::RequestProgramChannel::parseJson(const std::string &buffer) {
  const nlohmann::json packet = nlohmann::json::parse(buffer);
}

std::string Packet::RequestQueryProgram::buildJson() const {
  nlohmann::json packet;

  return packet.dump(-1, ' ', true);
}

void Packet::RequestQueryProgram::parseJson(const std::string &buffer) {
  const nlohmann::json packet = nlohmann::json::parse(buffer);
}

std::string Packet::ResponseQueryProgram::buildJson() const {
  nlohmann::json packet;

  auto &node = packet["list"];
  for (const auto &p : list) {
    nlohmann::json sub;
    sub["name"] = p.name;
    sub["publisher"] = p.publisher;
    sub["version"] = p.version;
    sub["install_date"] = p.install_date;
    sub["install_path"] = p.install_path;
    node.push_back(std::move(sub));
  }

  return packet.dump(-1, ' ', true);
}

void Packet::ResponseQueryProgram::parseJson(const std::string &buffer) {
  const nlohmann::json packet = nlohmann::json::parse(buffer);

  auto &node = packet["list"];
  for (const auto &sub : node) {
    Program p;
    p.name = sub["name"];
    p.publisher = sub["publisher"];
    p.version = sub["version"];
    p.install_date = sub["install_date"];
    p.install_path = sub["install_path"];
    list.push_back(std::move(p));
  }
}