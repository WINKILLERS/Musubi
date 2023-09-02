#pragma once
#include "AbstractPacket.h"

namespace Packet {
class RequestHeartbeatChannel : public AbstractPacket {
public:
  RequestHeartbeatChannel() = default;

  std::string buildJson() const override final;
  void parseJson(const std::string &buffer) override final;

  enum { PacketType = (uint32_t)Type::request_heartbeat_channel };
  inline Type getType() const override final { return (Type)PacketType; }
};

class ResponseHeartbeat : public AbstractPacket {
public:
  std::string focus_window;

  ResponseHeartbeat() = default;
  ResponseHeartbeat(const std::string &focus_window)
      : focus_window(focus_window) {}

  std::string buildJson() const override final;
  void parseJson(const std::string &buffer) override final;

  enum { PacketType = (uint32_t)Type::response_heartbeat };
  inline Type getType() const override final { return (Type)PacketType; }
};
} // namespace Packet
