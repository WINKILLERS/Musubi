#include "Factory.hpp"
#include "Handshake.hpp"
#include <exception>
#include <memory>

#define CASE_AND_PARSE(pt)                                                     \
  case (Type)pt::PacketType:                                                   \
    body = std::make_shared<pt>();                                             \
    break;

namespace Bridge {
bool Parser::parseJson(const std::string &buffer) {
  if (buffer.size() <= 2 * sizeof(uint64_t))
    return false;

  auto header_size = *(const uint64_t *)buffer.data();
  auto body_size = *((const uint64_t *)buffer.data() + 1);

  if (buffer.size() < 2 * sizeof(uint64_t) + header_size + body_size)
    return false;

  std::string header_data(buffer.data() + 2 * sizeof(uint64_t),
                          buffer.data() + 2 * sizeof(uint64_t) + header_size);
  std::string body_data(buffer.data() + 2 * sizeof(uint64_t) + header_size,
                        buffer.data() + 2 * sizeof(uint64_t) + header_size +
                            body_size);

  return parseJson(header_data, body_data);
}

bool Parser::parseJson(const std::string &header_data,
                       const std::string &body_data) {
  header = std::make_shared<Header>();

  try {
    header->parseJson(header_data);
  } catch (const std::exception &e) {
    return false;
  }

  // If packet's version mismatched with us
  if (header->version != getBridgeVersion()) {
    return false;
  }

  switch (header->type) {
    CASE_AND_PARSE(ClientHandshake)
    CASE_AND_PARSE(ServerHandshake)
  case Type::header:
  case Type::unknown:
  default:
    return false;
  }

  try {
    body->parseJson(body_data);
  } catch (const std::exception &e) {
    return false;
  }

  return true;
}
} // namespace Bridge