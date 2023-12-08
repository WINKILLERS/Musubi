#include "Factory.hpp"
#include "GetProcesses.hpp"
#include "Handshake.hpp"
#include "Heartbeat.hpp"
#include "RemoveFiles.hpp"
#include "StartProcess.hpp"
#include "TerminateProcess.hpp"
#include <GetFiles.hpp>
#include <exception>
#include <memory>
#include <zstd.h>

#define CASE_AND_PARSE(pt)                                                     \
  case (Type)pt::PacketType:                                                   \
    body = std::make_shared<pt>();                                             \
    break

namespace Bridge {
std::string compress(const std::string &data, const uint8_t level) {
  auto raw_size = data.size();
  std::string buffer;
  auto max_compress_size = ZSTD_compressBound(raw_size);

  buffer.resize(max_compress_size);

  auto actual_compress_size = ZSTD_compress(buffer.data(), max_compress_size,
                                            data.data(), raw_size, level);

  buffer.resize(actual_compress_size);

  return buffer;
}

std::string decompress(const std::string &data) {
  auto max_decompress_size = ZSTD_getFrameContentSize(data.data(), data.size());
  std::string buffer;

  buffer.resize(max_decompress_size);

  auto raw_size = ZSTD_decompress(buffer.data(), max_decompress_size,
                                  data.data(), data.size());

  buffer.resize(raw_size);

  return buffer;
}

std::string AbstractGenerator::generate(const std::string &header_data,
                                        const std::string &body_data) {
  std::string buffer;
  uint64_t header_size = header_data.size();
  uint64_t body_size = body_data.size();
  uint64_t raw_size = 2 * sizeof(uint64_t) + header_size + body_size;

  buffer.resize(raw_size);
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
    CASE_AND_PARSE(ClientHandshake);
    CASE_AND_PARSE(ServerHandshake);
    CASE_AND_PARSE(ClientInformation);
    CASE_AND_PARSE(Heartbeat);
    CASE_AND_PARSE(ResponseGetProcesses);
    CASE_AND_PARSE(RequestGetProcesses);
    CASE_AND_PARSE(ResponseTerminateProcess);
    CASE_AND_PARSE(RequestTerminateProcess);
    CASE_AND_PARSE(RequestStartProcess);
    CASE_AND_PARSE(ResponseStartProcess);
    CASE_AND_PARSE(RequestGetFiles);
    CASE_AND_PARSE(ResponseGetFiles);
    CASE_AND_PARSE(RequestRemoveFiles);
    CASE_AND_PARSE(ResponseRemoveFiles);
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
