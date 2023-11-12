#include "Handshake.hpp"
#include <Windows.h>
#include <catch2/catch_test_macros.hpp>

constexpr auto hwid = "test_id";

std::string getClientHandshakeData() {
  Bridge::ClientHandshake packet(hwid);
  return packet.buildJson();
}

Bridge::ClientHandshake parseClientHandshake(const std::string &data) {
  Bridge::ClientHandshake packet;
  packet.parseJson(data);
  return packet;
}

TEST_CASE("Build handshake packet", "[handshake][packets]") {
  auto data = getClientHandshakeData();
  auto packet = parseClientHandshake(data);

  REQUIRE(packet.hwid == hwid);
}

std::string getServerHandshakeData() {
  Bridge::ServerHandshake packet;
  return packet.buildJson();
}

Bridge::ServerHandshake parseServerHandshake(const std::string &data) {
  Bridge::ServerHandshake packet;
  packet.parseJson(data);
  return packet;
}

TEST_CASE("Build handshake packet", "[handshake][packets]") {
  auto data = getServerHandshakeData();
  auto packet = parseServerHandshake(data);

  REQUIRE(true);
}
