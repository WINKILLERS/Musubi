#include "UploadFile.h"
#include "AApch.h"

std::string Packet::RequestUploadFile::buildJson() const {
  nlohmann::json packet;

  packet["remote_path"] = remote_path;

  auto json = packet.dump(-1, ' ', true);
  ;

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

void Packet::RequestUploadFile::parseJson(const std::string &buffer) {
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

  remote_path = packet["remote_path"];
}

std::string Packet::ResponseUploadFile::buildJson() const {
  nlohmann::json packet;

  packet["error_code"] = error_code;

  return packet.dump(-1, ' ', true);
}

void Packet::ResponseUploadFile::parseJson(const std::string &buffer) {
  const nlohmann::json packet = nlohmann::json::parse(buffer);

  error_code = packet["error_code"];
}