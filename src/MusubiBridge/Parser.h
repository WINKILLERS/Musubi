#pragma once
#include "AbstractPacket.h"
#include "Header.h"
#include "memory"

namespace Packet {
class Parser {
public:
  std::shared_ptr<Header> header = nullptr;
  std::shared_ptr<AbstractPacket> body = nullptr;

  ~Parser();

  bool parseJson(const std::string &buffer);
  bool parseJson(const std::string &header_buffer,
                 const std::string &body_buffer);
};
} // namespace Packet