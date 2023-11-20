#include "Factory.hpp"
#include "Handshake.hpp"
#include <exception>
#include <memory>

#define CASE_AND_PARSE(pt)                                                     \
  case (Type)pt::PacketType:                                                   \
    body = std::make_shared<pt>();                                             \
    break;

namespace Bridge {
std::string AbstractGenerator::generate(const std::string &header_data,
                                        const std::string &body_data) {
  std::string buffer;
  uint64_t header_size = header_data.size();
  uint64_t body_size = body_data.size();

  buffer.resize(2 * sizeof(uint64_t) + header_size + body_size);
  auto data = buffer.data();

  // Copy header size
  memcpy(data, &header_size, sizeof(uint64_t));
  // Increase pointer
  data += sizeof(uint64_t);

  // Copy body size
  memcpy(data, &body_size, sizeof(uint64_t));
  // Increase pointer
  data += sizeof(uint64_t);

  // Copy header
  memcpy(data, header_data.data(), header_size);
  // Increase pointer
  data += header_size;

  // Copy body
  memcpy(data, body_data.data(), body_size);
  // Increase pointer
  data += body_size;

  return buffer;
}

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