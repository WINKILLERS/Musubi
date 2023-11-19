#ifndef FACTORY_HPP
#define FACTORY_HPP
#include "Packet.hpp"
#include <memory>
#include <string>
#include <type_traits>

#define GENERATE_PACKET(t, ...) Bridge::Generator<t>((t(__VA_ARGS__)));
#define GENERATE_PACKET_WITH_ID(t, id, ...)                                    \
  Bridge::Generator<t>((t(__VA_ARGS__)), id);

namespace Bridge {
using HeaderPtr = std::shared_ptr<Header>;
using BodyPtr = std::shared_ptr<AbstractPacket>;

class AbstractGenerator {
public:
  virtual ~AbstractGenerator() = default;

  virtual std::string buildJson() const = 0;
};

template <typename T>
concept Packet = std::is_base_of_v<AbstractPacket, T>;

template <typename T>
  requires Packet<T>
class Generator : public AbstractGenerator {
public:
  Generator(T &&packet, const std::string &id = "Default")
      : header((Type)T::PacketType, id), body(packet){};
  virtual ~Generator() = default;

  std::string buildJson() const override;

private:
  T body;
  Header header;
};

template <typename T>
  requires Packet<T>
std::string Generator<T>::buildJson() const {
  std::string buffer;

  auto header_data = header.buildJson();
  auto body_data = body.buildJson();
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

class Parser final {
public:
  bool parseJson(const std::string &buffer);
  bool parseJson(const std::string &header_data, const std::string &body_data);

  inline HeaderPtr getHeader() const { return header; };
  inline BodyPtr getBody() const { return body; };
  template <typename T> std::shared_ptr<T> getBody() const {
    return std::dynamic_pointer_cast<T>(body);
  };

private:
  HeaderPtr header;
  BodyPtr body;
};
} // namespace Bridge
#endif