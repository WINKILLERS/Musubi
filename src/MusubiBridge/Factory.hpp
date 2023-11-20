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
  virtual void setId(const uint64_t id) = 0;
  virtual uint64_t getId() const = 0;

protected:
  static std::string generate(const std::string &header_data,
                              const std::string &body_data);
};

template <typename T>
concept Packet = std::is_base_of_v<AbstractPacket, T>;

template <typename T>
  requires Packet<T>
class Generator : public AbstractGenerator {
public:
  Generator(T &&packet, const uint64_t id = 0)
      : header((Type)T::PacketType, id), body(packet){};
  virtual ~Generator() = default;

  std::string buildJson() const override;
  inline void setId(const uint64_t id) override { header.id = id; };
  inline uint64_t getId() const override { return header.id; };

private:
  T body;
  Header header;
};

template <typename T>
  requires Packet<T>
std::string Generator<T>::buildJson() const {
  auto header_data = header.buildJson();
  auto body_data = body.buildJson();

  return generate(header_data, body_data);
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