#pragma once
#include "AbstractPacket.h"
#include "vector"

namespace Packet {
class RequestScreenChannel : public AbstractPacket {
public:
  bool is_hidden;

  RequestScreenChannel(const bool &is_hidden = false) : is_hidden(is_hidden) {}

  std::string buildJson() const override final;
  void parseJson(const std::string &buffer) override final;

  enum { PacketType = (uint32_t)Type::request_screen_channel };
  inline Type getType() const override final { return (Type)PacketType; }
};

class RequestRemoteScreenSetArgs : public AbstractPacket {
public:
  uint8_t compression_level;

  RequestRemoteScreenSetArgs(const uint8_t &compression_level = 3)
      : compression_level(compression_level) {}

  std::string buildJson() const override final;
  void parseJson(const std::string &buffer) override final;

  enum { PacketType = (uint32_t)Type::request_remote_screen_set_args };
  inline Type getType() const override final { return (Type)PacketType; }

  inline bool operator==(const RequestRemoteScreenSetArgs &other) const {
    return compression_level == other.compression_level;
  }
};

class RequestInputChannel : public AbstractPacket {
public:
  RequestInputChannel() = default;

  std::string buildJson() const override final;
  void parseJson(const std::string &buffer) override final;

  enum { PacketType = (uint32_t)Type::request_input_channel };
  inline Type getType() const override final { return (Type)PacketType; }
};

class RequestSetMouse : public AbstractPacket {
public:
  enum class MouseAction : uint8_t {
    Move,
    LeftPressed,
    LeftReleased,
    RightPressed,
    RightReleased,
    LeftDoubleClick,
    RightDoubleClick,
    WheelMove,
    MiddlePressed,
    MiddleReleased,
  };

  struct MouseInput {
    MouseAction action;
    double x_percentage;
    double y_percentage;
    int8_t wheel_delta;

    inline bool operator==(const MouseInput &other) const {
      return wheel_delta == other.wheel_delta &&
             x_percentage == other.x_percentage &&
             y_percentage == other.y_percentage && action == other.action;
    }
  };

  std::vector<MouseInput> inputs;

  RequestSetMouse() = default;
  RequestSetMouse(const std::vector<MouseInput> &inputs) : inputs(inputs) {}

  std::string buildJson() const override final;
  void parseJson(const std::string &buffer) override final;

  enum { PacketType = (uint32_t)Type::request_set_mouse };
  inline Type getType() const override final { return (Type)PacketType; }

  inline bool operator==(const RequestSetMouse &other) const {
    if (inputs.size() != other.inputs.size())
      return false;

    for (size_t i = 0; i < inputs.size(); i++) {
      auto &t1 = inputs[i];
      auto &t2 = other.inputs[i];

      if (t1.action != t2.action)
        return false;
      if (t1.x_percentage != t2.x_percentage)
        return false;
      if (t1.y_percentage != t2.y_percentage)
        return false;
    }

    return true;
  }
};

class RequestSetKeyboard : public AbstractPacket {
public:
  enum class KeyboardAction : uint8_t {
    Pressed,
    Released,
  };

  struct KeyBoardInput {
    KeyboardAction action;
    uint8_t virtual_code;
  };

  std::vector<KeyBoardInput> inputs;

  RequestSetKeyboard() = default;
  RequestSetKeyboard(const std::vector<KeyBoardInput> &inputs)
      : inputs(inputs) {}

  std::string buildJson() const override final;
  void parseJson(const std::string &buffer) override final;

  enum { PacketType = (uint32_t)Type::request_set_keyboard };
  inline Type getType() const override final { return (Type)PacketType; }

  inline bool operator==(const RequestSetKeyboard &other) const {
    if (inputs.size() != other.inputs.size())
      return false;

    for (size_t i = 0; i < inputs.size(); i++) {
      auto &t1 = inputs[i];
      auto &t2 = other.inputs[i];

      if (t1.action != t2.action)
        return false;
      if (t1.virtual_code != t2.virtual_code)
        return false;
    }

    return true;
  }
};

class ResponseRemoteScreen : public AbstractPacket {
public:
  struct ScreenRect {
    uint32_t x;
    uint32_t y;
    uint32_t width;
    uint32_t height;
    std::string screen;
  };

  ScreenRect rect;

  ResponseRemoteScreen() = default;
  ResponseRemoteScreen(const uint32_t &x, const uint32_t &y,
                       const uint32_t &width, const uint32_t &height,
                       const std::string &screen,
                       const uint8_t &compress_level = 3)
      : rect(x, y, width, height, screen), compress_level(compress_level) {}
  ResponseRemoteScreen(const uint32_t &x, const uint32_t &y,
                       const uint32_t &width, const uint32_t &height,
                       std::string &&screen, const uint8_t &compress_level = 3)
      : rect(x, y, width, height, std::move(screen)),
        compress_level(compress_level) {}
  ResponseRemoteScreen(const ScreenRect &rect,
                       const uint8_t &compress_level = 3)
      : rect(rect), compress_level(compress_level) {}
  ResponseRemoteScreen(ScreenRect &&rect, const uint8_t &compress_level = 3)
      : rect(std::move(rect)), compress_level(compress_level) {}

  std::string buildJson() const override final;
  void parseJson(const std::string &buffer) override final;

  enum { PacketType = (uint32_t)Type::response_remote_screen };
  inline Type getType() const override final { return (Type)PacketType; }

  inline bool operator==(const ResponseRemoteScreen &other) const {
    return rect.width == other.rect.width && rect.height == other.rect.height &&
           rect.screen == other.rect.screen && rect.x == other.rect.x &&
           rect.y == other.rect.y;
  }

private:
  uint8_t compress_level;
};
} // namespace Packet