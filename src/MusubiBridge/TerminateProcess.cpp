#include "TerminateProcess.hpp"
#include <nlohmann/json.hpp>

namespace Bridge {
std::string RequestTerminateProcess::buildJson() const {
  nlohmann::json packet;

  for (const auto &pid : processes) {
    nlohmann::json process_json;
    process_json["pid"] = pid;
    packet.emplace_back(process_json);
  }

  return packet.dump(-1, ' ', true);
}

void RequestTerminateProcess::parseJson(const std::string &json) {
  const nlohmann::json packet = nlohmann::json::parse(json);

  for (const auto &process_json : packet) {
    uint64_t pid;
    pid = process_json["pid"];
    processes.emplace_back(pid);
  }
}

std::string ResponseTerminateProcess::buildJson() const {
  nlohmann::json packet;

  for (const auto &status : statuses) {
    nlohmann::json status_json;
    status_json["pid"] = status.pid;
    status_json["success"] = status.success;
    packet.emplace_back(status_json);
  }

  return packet.dump(-1, ' ', true);
}

void ResponseTerminateProcess::parseJson(const std::string &json) {
  const nlohmann::json packet = nlohmann::json::parse(json);

  for (const auto &status_json : packet) {
    TerminateStatus status;
    status.pid = status_json["pid"];
    status.success = status_json["success"];
    statuses.emplace_back(status);
  }
}
} // namespace Bridge