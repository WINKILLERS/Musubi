#include "GetProcess.h"
#include "AApch.h"

std::string Packet::RequestGetProcess::buildJson() const {
  nlohmann::json packet;

  return packet.dump(-1, ' ', true);
}

void Packet::RequestGetProcess::parseJson(const std::string &buffer) {
  const nlohmann::json packet = nlohmann::json::parse(buffer);
}

std::string Packet::ResponseGetProcess::buildJson() const {
  nlohmann::json packet;

  auto &node = packet["list"];
  for (const auto &p : list) {
    nlohmann::json sub;
    sub["pid"] = p.pid;
    sub["name"] = p.name;
    sub["ppid"] = p.ppid;
    node.push_back(std::move(sub));
  }

  return packet.dump(-1, ' ', true);
}

void Packet::ResponseGetProcess::parseJson(const std::string &buffer) {
  const nlohmann::json packet = nlohmann::json::parse(buffer);

  auto &node = packet["list"];
  for (const auto &sub : node) {
    Process p;
    p.pid = sub["pid"];
    p.name = sub["name"];
    p.ppid = sub["ppid"];
    list.push_back(std::move(p));
  }
}