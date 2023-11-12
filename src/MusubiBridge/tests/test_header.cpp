#include "Packet.hpp"
#include <Windows.h>
#include <catch2/catch_test_macros.hpp>

constexpr auto type = Bridge::Type::unknown;
constexpr auto id = "test_id";

std::string getHeaderData() {
  Bridge::Header packet(type, id);
  return packet.buildJson();
}

Bridge::Header parseHeader(const std::string &data) {
  Bridge::Header packet;
  packet.parseJson(data);
  return packet;
}

TEST_CASE("Build header packet", "[header][packets]") {
  auto data = getHeaderData();
  auto packet = parseHeader(data);

  REQUIRE(packet.id == id);
  REQUIRE(packet.type == type);
  REQUIRE(packet.timestamp != 0);
  REQUIRE(packet.version == Bridge::getBridgeVersion());
}
