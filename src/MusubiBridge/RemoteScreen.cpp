#include "RemoteScreen.h"
#include "AApch.h"
#include "zstd.h"

std::string Packet::RequestScreenChannel::buildJson() const {
  nlohmann::json packet;

  EMPLACE_PARAM(is_hidden);

  return packet.dump(-1, ' ', true);
}

void Packet::RequestScreenChannel::parseJson(const std::string &buffer) {
  const nlohmann::json packet = nlohmann::json::parse(buffer);

  EXTRACT_PARAM(is_hidden);
}

std::string Packet::RequestRemoteScreenSetArgs::buildJson() const {
  nlohmann::json packet;

  EMPLACE_PARAM(compression_level);

  return packet.dump(-1, ' ', true);
}

void Packet::RequestRemoteScreenSetArgs::parseJson(const std::string &buffer) {
  const nlohmann::json packet = nlohmann::json::parse(buffer);

  EXTRACT_PARAM(compression_level);
}

std::string Packet::RequestInputChannel::buildJson() const {
  nlohmann::json packet;

  return packet.dump(-1, ' ', true);
}

void Packet::RequestInputChannel::parseJson(const std::string &buffer) {
  const nlohmann::json packet = nlohmann::json::parse(buffer);
}

std::string Packet::RequestSetMouse::buildJson() const {
  nlohmann::json packet;

  auto &node = packet["mouse"];

  for (const auto &input : inputs) {
    nlohmann::json input_object;

    input_object["wheel_delta"] = input.wheel_delta;
    input_object["x"] = input.x_percentage;
    input_object["y"] = input.y_percentage;
    input_object["action"] = input.action;

    node.push_back(input_object);
  }

  return packet.dump(-1, ' ', true);
}

void Packet::RequestSetMouse::parseJson(const std::string &buffer) {
  const nlohmann::json packet = nlohmann::json::parse(buffer);

  auto &node = packet["mouse"];

  for (const auto &input_object : node) {
    MouseInput input;

    input.wheel_delta = input_object["wheel_delta"];
    input.x_percentage = input_object["x"];
    input.y_percentage = input_object["y"];
    input.action = input_object["action"];

    inputs.push_back(input);
  }
}

std::string Packet::RequestSetKeyboard::buildJson() const {
  nlohmann::json packet;

  auto &node = packet["keyboard"];

  for (const auto &input : inputs) {
    nlohmann::json input_object;

    input_object["action"] = input.action;
    input_object["virtual_code"] = input.virtual_code;

    node.push_back(input_object);
  }

  return packet.dump(-1, ' ', true);
}

void Packet::RequestSetKeyboard::parseJson(const std::string &buffer) {
  const nlohmann::json packet = nlohmann::json::parse(buffer);

  auto &node = packet["keyboard"];

  for (const auto &input_object : node) {
    KeyBoardInput input;

    input.action = input_object["action"];
    input.virtual_code = input_object["virtual_code"];

    inputs.push_back(input);
  }
}

std::string Packet::ResponseRemoteScreen::buildJson() const {
  auto required_size = ZSTD_compressBound(screen.size());

  std::string buffer;
  buffer.resize(required_size + 4 * sizeof(uint32_t));
  *((uint32_t *)buffer.data() + 0) = x;
  *((uint32_t *)buffer.data() + 1) = y;
  *((uint32_t *)buffer.data() + 2) = width;
  *((uint32_t *)buffer.data() + 3) = height;
  auto actual_size =
      ZSTD_compress(buffer.data() + 4 * sizeof(uint32_t), required_size,
                    screen.data(), screen.size(), compression_level);
  buffer.resize(actual_size + 4 * sizeof(uint32_t));

  return buffer;
}

void Packet::ResponseRemoteScreen::parseJson(const std::string &buffer) {
  x = *((uint32_t *)buffer.data() + 0);
  y = *((uint32_t *)buffer.data() + 1);
  width = *((uint32_t *)buffer.data() + 2);
  height = *((uint32_t *)buffer.data() + 3);
  auto required_size =
      ZSTD_getFrameContentSize(buffer.data() + 4 * sizeof(uint32_t),
                               buffer.size() - 4 * sizeof(uint32_t));
  screen.resize(required_size);
  auto actual_size = ZSTD_decompress(screen.data(), required_size,
                                     buffer.data() + 4 * sizeof(uint32_t),
                                     buffer.size() - 4 * sizeof(uint32_t));
  screen.resize(actual_size);
}