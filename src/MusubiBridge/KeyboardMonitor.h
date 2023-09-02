#pragma once
#include "AbstractPacket.h"

namespace Packet {
class RequestKeyboardMonitorChannel : public AbstractPacket {
public:
  RequestKeyboardMonitorChannel() = default;

  std::string buildJson() const override final;
  void parseJson(const std::string &buffer) override final;

  enum { PacketType = (uint32_t)Type::request_keyboard_monitor_channel };
  inline Type getType() const override final { return (Type)PacketType; }
};

class ResponseGetKey : public AbstractPacket {
public:
  enum class KeyboardAction : uint8_t {
    Pressed,
    Released,
  };

  struct KeyBoardInput {
    KeyboardAction action;
    std::string keys;
    uint32_t virtual_code;
  };

  KeyBoardInput input;

  ResponseGetKey() = default;
  ResponseGetKey(const KeyBoardInput &input) : input(input) {}

  std::string buildJson() const override final;
  void parseJson(const std::string &buffer) override final;

  enum { PacketType = (uint32_t)Type::response_get_key };
  inline Type getType() const override final { return (Type)PacketType; }
};
} // namespace Packet