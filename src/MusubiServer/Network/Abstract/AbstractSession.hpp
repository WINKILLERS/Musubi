#ifndef ABSTRACT_SESSION_HPP
#define ABSTRACT_SESSION_HPP
#include "Factory.hpp"
#include "Handshake.hpp"
#include <QObject>
#include <string>

namespace Network {
enum class Type : uint8_t { tcp };

class PacketNotifier : public QObject {
  Q_OBJECT;

public:
  bool dispatch(const Bridge::Parser &parser);

signals:
};

class AbstractSession {
public:
  // Get session type
  virtual Type getType() const = 0;
  // Get session role
  inline Bridge::Role getRole() const { return role; };

  // Get remote ip address
  virtual std::string getRemoteAddress() const = 0;
  // Get remote port
  virtual uint16_t getRemotePort() const = 0;

  // Disconnect from remote
  virtual void shutdown() = 0;

  // Send json packet
  bool sendJsonPacket(const Bridge::AbstractGenerator &gen);

  // Get the hwid for this session
  inline std::string getHwid() const { return hwid; }

protected:
  // Set the hwid for this session
  inline void setHwid(const std::string &hwid_) { hwid = hwid_; }

  // When received packet
  bool processPacket(const std::string &buffer);

private:
  // hwid for the session
  std::string hwid;

  // Session's role
  Bridge::Role role = Bridge::Role::unknown;
};
} // namespace Network
#endif