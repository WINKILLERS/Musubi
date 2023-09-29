#pragma once
#include "AbstractPacket.h"
#include "Header.h"

namespace Packet {
class AbstractGenerator {
public:
  static const uint64_t magic = 0x123456789ABCEF00;

  Header header;

  AbstractGenerator(const Type &type, const std::string &id,
                    const uint64_t &timestamp = 0);

  // We do not allow copy
  AbstractGenerator(const AbstractGenerator &) = delete;
  AbstractGenerator &operator=(const AbstractGenerator &) = delete;
  virtual ~AbstractGenerator() = default;

  virtual std::string buildJson() const = 0;

  inline AbstractGenerator &setId(const std::string &id = "") {
    header.id = id;
    return *this;
  }
  inline std::string getId(const std::string &id = constructId()) const {
    return header.id;
  }

  static std::string constructId();
};

template <typename PacketType> class Generator : public AbstractGenerator {
public:
  PacketType body;

  Generator();
  template <class... _Valty> Generator(_Valty &&..._Val);
  Generator(const Generator &) = delete;

  // We do not allow copy
  Generator &operator=(const Generator &) = delete;

  std::string buildJson() const;
};

template <typename PacketType>
inline Generator<PacketType>::Generator()
    : AbstractGenerator(body.getType(), constructId()), body() {}

template <typename PacketType>
template <class... _Valty>
inline Generator<PacketType>::Generator(_Valty &&..._Val)
    : AbstractGenerator(body.getType(), constructId()),
      body(std::forward<_Valty>(_Val)...) {}

template <typename PacketType>
inline std::string Generator<PacketType>::buildJson() const {
  std::string buffer;

  auto header_buffer = header.buildJson();
  auto body_buffer = body.buildJson();
  uint64_t header_size = header_buffer.size();
  uint64_t body_size = body_buffer.size();

  buffer.resize(3 * sizeof(uint64_t) + header_size + body_size);
  auto data = buffer.data();

  // Copy magic
  memcpy(data, &magic, sizeof(uint64_t));
  // Increase pointer
  data += sizeof(uint64_t);

  // Copy header size
  memcpy(data, &header_size, sizeof(uint64_t));
  // Increase pointer
  data += sizeof(uint64_t);

  // Copy body size
  memcpy(data, &body_size, sizeof(uint64_t));
  // Increase pointer
  data += sizeof(uint64_t);

  // Copy header
  memcpy(data, header_buffer.data(), header_size);
  // Increase pointer
  data += header_size;

  // Copy body
  memcpy(data, body_buffer.data(), body_size);
  // Increase pointer
  data += body_size;

  return buffer;
}
} // namespace Packet