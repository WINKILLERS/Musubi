#include "DownloadFile.h"
#include "AApch.h"

std::string Packet::RequestDownloadFile::buildJson() const {
  nlohmann::json packet;

  EMPLACE_PARAM(path);

  return packet.dump(-1, ' ', true);
}

void Packet::RequestDownloadFile::parseJson(const std::string &buffer) {
  const nlohmann::json packet = nlohmann::json::parse(buffer);

  path = packet["path"];
}

std::string Packet::ResponseDownloadFile::buildJson() const {
  nlohmann::json packet;

  EMPLACE_PARAM(error_code);

  auto json = packet.dump(-1, ' ', true);

  std::string buffer;
  buffer.resize(sizeof(uint32_t) + sizeof(uint64_t) + json.size() +
                content.size());
  *((uint32_t *)buffer.data() + 0) = (uint32_t)json.size();
  *((uint64_t *)((uint32_t *)buffer.data() + 1)) = content.size();
  std::copy(json.cbegin(), json.cend(),
            buffer.data() + sizeof(uint32_t) + sizeof(uint64_t));
  memcpy(buffer.data() + sizeof(uint32_t) + sizeof(uint64_t) + json.size(),
         content.data(), content.size());

  return buffer;
}

void Packet::ResponseDownloadFile::parseJson(const std::string &buffer) {
  auto json_size = *((uint32_t *)buffer.data() + 0);
  auto content_size = *((uint64_t *)((uint32_t *)buffer.data() + 1));

  std::string json;
  json.reserve(json_size);
  content.resize(content_size);

  std::copy(buffer.data() + sizeof(uint32_t) + sizeof(uint64_t),
            buffer.data() + sizeof(uint32_t) + sizeof(uint64_t) + json_size,
            std::back_inserter(json));
  memcpy(content.data(),
         buffer.data() + sizeof(uint32_t) + sizeof(uint64_t) + json_size,
         content_size);

  const nlohmann::json packet = nlohmann::json::parse(json);

  EXTRACT_PARAM(error_code);
}