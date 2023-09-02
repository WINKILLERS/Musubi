#pragma once
#include "AbstractPacket.h"

namespace Packet {
class RequestInformation : public AbstractPacket {
public:
  RequestInformation() = default;

  std::string buildJson() const override final;
  void parseJson(const std::string &buffer) override final;

  enum { PacketType = (uint32_t)Type::request_information };
  inline Type getType() const override final { return (Type)PacketType; }
};

class ResponseInformation : public AbstractPacket {
public:
  std::string cpu_model;
  std::string os_name;
  std::string user_name;
  std::string computer_name;

  ResponseInformation() = default;
  ResponseInformation(const std::string &cpu_model, const std::string &os_name,
                      const std::string &user_name,
                      const std::string &computer_name)
      : cpu_model(cpu_model), os_name(os_name), user_name(user_name),
        computer_name(computer_name) {}

  std::string buildJson() const override final;
  void parseJson(const std::string &buffer) override final;

  enum { PacketType = (uint32_t)Type::response_information };
  inline Type getType() const override final { return (Type)PacketType; }

  inline bool operator==(const ResponseInformation &other) const {
    return cpu_model == other.cpu_model && os_name == other.os_name &&
           user_name == other.user_name && computer_name == other.computer_name;
  }
};
} // namespace Packet