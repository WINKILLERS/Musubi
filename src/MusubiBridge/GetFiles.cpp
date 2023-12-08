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

  EMPLACE_PARAM(error_code);

  auto &node = packet["files"];
  for (const auto &file : files) {
    nlohmann::json file_json;
    file_json["name"] = file.name;
    file_json["is_directory"] = file.is_directory;
    file_json["size"] = file.size;
    file_json["last_write_time"] = file.last_write_time;
    node.emplace_back(file_json);
  }

  return packet.dump(-1, ' ', true);
}

void ResponseGetFiles::parseJson(const std::string &json) {
  const nlohmann::json packet = nlohmann::json::parse(json);

  EXTRACT_PARAM(error_code);

  auto &node = packet["files"];
  for (const auto &file_json : node) {
    File file;
    file.name = file_json["name"];
    file.is_directory = file_json["is_directory"];
    file.size = file_json["size"];
    file.last_write_time = file_json["last_write_time"];
    files.emplace_back(file);
  }
}
} // namespace Bridge
