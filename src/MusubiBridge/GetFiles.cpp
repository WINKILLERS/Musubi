#include "GetFiles.hpp"
#include <nlohmann/json.hpp>

namespace Bridge {
std::string RequestGetFiles::buildJson() const {
  nlohmann::json packet;

  EMPLACE_PARAM(directory);

  return packet.dump(-1, ' ', true);
}

void RequestGetFiles::parseJson(const std::string &json) {
  const nlohmann::json packet = nlohmann::json::parse(json);

  EXTRACT_PARAM(directory);
}

std::string ResponseGetFiles::buildJson() const {
  nlohmann::json packet;

  for (const auto &file : files) {
    nlohmann::json file_json;
    file_json["name"] = file.name;
    packet.emplace_back(file_json);
  }

  return packet.dump(-1, ' ', true);
}

void ResponseGetFiles::parseJson(const std::string &json) {
  const nlohmann::json packet = nlohmann::json::parse(json);

  for (const auto &file_json : packet) {
    File file;
    file.name = file_json["name"];
    files.emplace_back(file);
  }
}
} // namespace Bridge