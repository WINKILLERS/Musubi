#ifndef ABSTRACT_SESSION_HPP
#define ABSTRACT_SESSION_HPP
#include "Common.hpp"
#include "Factory.hpp"
#include <QObject>
#include <string>

namespace Network {
// Forward declare
class AbstractHandler;

// Session type
enum class Type : uint8_t { tcp };

class PacketNotifier final : public QObject {
  Q_OBJECT;

public:
  bool dispatch(const Bridge::Parser &parser);

signals:
  void lag(uint32_t lag);
};

class AbstractSession : public IDeleteSelf {
public:
  AbstractSession(AbstractHandler *handler_);
  virtual ~AbstractSession();

  // Get session type
  virtual Type getType() const = 0;
  // Get session role
  inline Bridge::Role getRole() const { return role; };
  // Get session's description
  std::string getDescription() const;

  // Get remote ip address
  virtual std::string getRemoteAddress() const = 0;
  // Get remote port
  virtual uint16_t getRemotePort() const = 0;

  // Disconnect from remote
  virtual void shutdown() = 0;

  // Send json packet
  virtual bool sendJsonPacket(const Bridge::AbstractGenerator &gen) = 0;

  // Get the hwid for this session
  inline std::string getHwid() const { return hwid; }

  inline uint64_t getHandshakeId() const { return handshake_id; }

  PacketNotifier notifier;

protected:
  // Set the hwid for this session
  inline void setHwid(const std::string &hwid_) { hwid = hwid_; }

  // When received packet
  bool processPacket(const std::string &buffer);

private:
  // hwid for the session
  std::string hwid;

  uint64_t handshake_id;

  // Session's role
  Bridge::Role role = Bridge::Role::unknown;

  // Who created this session
  AbstractHandler *handler;
};
} // namespace Network
#endif