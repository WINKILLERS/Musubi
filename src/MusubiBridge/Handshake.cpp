#include "Handshake.h"
#include "AApch.h"

std::string Packet::Handshake::buildJson() const {
  nlohmann::json packet;

  packet["role"] = role;
  packet["message"] = message;

  return packet.dump(-1, ' ', true);
}

void Packet::Handshake::parseJson(const std::string &buffer) {
  const nlohmann::json packet = nlohmann::json::parse(buffer);

  role = packet["role"];
  message = packet["message"];
}

std::string Packet::Disconnect::buildJson() const {
  nlohmann::json packet;

  packet["action"] = action;

  return packet.dump(-1, ' ', true);
}

void Packet::Disconnect::parseJson(const std::string &buffer) {
  const nlohmann::json packet = nlohmann::json::parse(buffer);

  action = packet["action"];
}

std::string Packet::RequestReinitialize::buildJson() const {
  nlohmann::json packet;

  return packet.dump(-1, ' ', true);
}

void Packet::RequestReinitialize::parseJson(const std::string &buffer) {
  const nlohmann::json packet = nlohmann::json::parse(buffer);
}

std::string Packet::ResponseReinitialize::buildJson() const {
  nlohmann::json packet;

  packet["error_code"] = error_code;

  return packet.dump(-1, ' ', true);
}

void Packet::ResponseReinitialize::parseJson(const std::string &buffer) {
  const nlohmann::json packet = nlohmann::json::parse(buffer);

  error_code = packet["error_code"];
}