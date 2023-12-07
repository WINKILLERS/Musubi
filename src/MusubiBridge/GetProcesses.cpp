#include "GetProcesses.hpp"
#include <nlohmann/json.hpp>

namespace Bridge {
std::string ResponseGetProcesses::buildJson() const {
  nlohmann::json packet;

  for (const auto &process : processes) {
    nlohmann::json process_json;
    process_json["name"] = process.name;
    process_json["pid"] = process.pid;
    process_json["ppid"] = process.ppid;
    packet.emplace_back(process_json);
  }

  return packet.dump(-1, ' ', true);
}

void ResponseGetProcesses::parseJson(const std::string &json) {
  const nlohmann::json packet = nlohmann::json::parse(json);

  for (const auto &process_json : packet) {
    Process process;
    process.name = process_json["name"];
    process.pid = process_json["pid"];
    process.ppid = process_json["ppid"];
    processes.emplace_back(process);
  }
}
} // namespace Bridge