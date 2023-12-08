#include "StartProcess.hpp"
#include <nlohmann/json.hpp>

namespace Bridge {
std::string RequestStartProcess::buildJson() const {
  nlohmann::json packet;

  EMPLACE_PARAM(path);
  EMPLACE_PARAM(show);

  return packet.dump(-1, ' ', true);
}

void RequestStartProcess::parseJson(const std::string &json) {
  const nlohmann::json packet = nlohmann::json::parse(json);

  EXTRACT_PARAM(path);
  EXTRACT_PARAM(show);
}

std::string ResponseStartProcess::buildJson() const {
  nlohmann::json packet;

  packet["path"] = status.path;
  packet["ec"] = status.error_code;

  return packet.dump(-1, ' ', true);
}

void ResponseStartProcess::parseJson(const std::string &json) {
  const nlohmann::json packet = nlohmann::json::parse(json);

  status.path = packet["path"];
  status.error_code = packet["ec"];
}
} // namespace Bridge