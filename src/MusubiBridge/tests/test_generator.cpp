#include "Factory.hpp"
#include "Handshake.hpp"
#include <Windows.h>
#include <catch2/catch_test_macros.hpp>
#include <vector>

constexpr auto hwid = "test_id";
constexpr auto id = 114514;

std::vector<std::string> getPacketData() {
  std::vector<std::string> ret;

  {
    auto gen = GENERATE_PACKET_WITH_ID(Bridge::ClientHandshake, id, hwid,
                                       Bridge::Role::controller);
    ret.push_back(gen.buildJson());
  }

  {
    auto gen = GENERATE_PACKET_WITH_ID(Bridge::ServerHandshake, id);
    ret.push_back(gen.buildJson());
  }

  return ret;
}

Bridge::Parser parsePacket(const std::string &data) {
  Bridge::Parser parser;
  auto ret = parser.parseJson(data);
  REQUIRE(ret == true);
  return parser;
}

TEST_CASE("Build packet", "[generator][packets]") {
  auto data_set = getPacketData();

  for (const auto &data : data_set) {
    auto parser = parsePacket(data);
    auto header = parser.getHeader();
    auto body = parser.getBody();

    REQUIRE(body != nullptr);
    REQUIRE(header->id == id);
    REQUIRE(header->type == body->getType());
    REQUIRE(header->timestamp != 0);
    REQUIRE(header->version == Bridge::getBridgeVersion());
  }
}
