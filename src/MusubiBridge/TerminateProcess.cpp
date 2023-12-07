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

  for (const auto &status : processes) {
    nlohmann::json process_json;
    process_json["pid"] = status.pid;
    process_json["success"] = status.success;
    packet.emplace_back(process_json);
  }

  return packet.dump(-1, ' ', true);
}

void ResponseTerminateProcess::parseJson(const std::string &json) {
  const nlohmann::json packet = nlohmann::json::parse(json);

  for (const auto &process_json : packet) {
    TerminateStatus status;
    status.pid = process_json["pid"];
    status.success = process_json["success"];
    processes.emplace_back(status);
  }
}
} // namespace Bridge