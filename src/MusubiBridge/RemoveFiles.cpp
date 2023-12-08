#include "RemoveFiles.hpp"
#include <nlohmann/json.hpp>

namespace Bridge {
std::string RequestRemoveFiles::buildJson() const {
  nlohmann::json packet;

  for (const auto &path : paths) {
    nlohmann::json path_json;
    path_json["path"] = path;
    packet.emplace_back(path_json);
  }

  return packet.dump(-1, ' ', true);
}

void RequestRemoveFiles::parseJson(const std::string &json) {
  const nlohmann::json packet = nlohmann::json::parse(json);

  for (const auto &path_json : packet) {
    std::string path;
    path = path_json["path"];
    paths.emplace_back(path);
  }
}

std::string ResponseRemoveFiles::buildJson() const {
  nlohmann::json packet;

  for (const auto &status : statuses) {
    nlohmann::json status_json;
    status_json["path"] = status.path;
    status_json["ec"] = status.error_code;
    packet.emplace_back(status_json);
  }

  return packet.dump(-1, ' ', true);
}

void ResponseRemoveFiles::parseJson(const std::string &json) {
  const nlohmann::json packet = nlohmann::json::parse(json);

  for (const auto &status_json : packet) {
    RemoveStatus status;
    status.path = status_json["path"];
    status.error_code = status_json["ec"];
    statuses.emplace_back(status);
  }
}
} // namespace Bridge
