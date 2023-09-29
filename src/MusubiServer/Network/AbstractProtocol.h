#pragma once
#include "Generator.h"
#include "Parser.h"
#include "Protocols.h"
#include "magic_enum.hpp"
#include "qobject.h"
#include "qpromise.h"
#include "unordered_map"
#include <utility>

using namespace magic_enum::bitwise_operators;

namespace Network {
// Forward declare
class AbstractSession;
class AbstractChannel;
class Controller;

// Define notifies emit when received packet
class IPacketNotify : public QObject {
  Q_OBJECT

public:
  void dispatch(const Packet::Parser &parser);

signals:
  void recvHeartbeat(std::shared_ptr<Packet::Header> header,
                     std::shared_ptr<Packet::ResponseHeartbeat> packet);
  void recvInformation(std::shared_ptr<Packet::Header> header,
                       std::shared_ptr<Packet::ResponseInformation> packet);
  void recvDeleteFile(std::shared_ptr<Packet::Header> header,
                      std::shared_ptr<Packet::ResponseDeleteFile> packet);
  void recvDownloadFile(std::shared_ptr<Packet::Header> header,
                        std::shared_ptr<Packet::ResponseDownloadFile> packet);
  void recvExecuteFile(std::shared_ptr<Packet::Header> header,
                       std::shared_ptr<Packet::ResponseExecuteFile> packet);
  void recvGetProcess(std::shared_ptr<Packet::Header> header,
                      std::shared_ptr<Packet::ResponseGetProcess> packet);
  void recvQueryFile(std::shared_ptr<Packet::Header> header,
                     std::shared_ptr<Packet::ResponseQueryFile> packet);
  void recvQueryProgram(std::shared_ptr<Packet::Header> header,
                        std::shared_ptr<Packet::ResponseQueryProgram> packet);
  void recvReinitialize(std::shared_ptr<Packet::Header> header,
                        std::shared_ptr<Packet::ResponseReinitialize> packet);
  void recvRemoteScreen(std::shared_ptr<Packet::Header> header,
                        std::shared_ptr<Packet::ResponseRemoteScreen> packet);
  void recvStartProcess(std::shared_ptr<Packet::Header> header,
                        std::shared_ptr<Packet::ResponseStartProcess> packet);
  void recvTaskAutoStart(std::shared_ptr<Packet::Header> header,
                         std::shared_ptr<Packet::ResponseTaskAutoStart> packet);
  void recvTerminateProcess(
      std::shared_ptr<Packet::Header> header,
      std::shared_ptr<Packet::ResponseTerminateProcess> packet);
  void recvUploadFile(std::shared_ptr<Packet::Header> header,
                      std::shared_ptr<Packet::ResponseUploadFile> packet);
  void recvGetKey(std::shared_ptr<Packet::Header> header,
                  std::shared_ptr<Packet::ResponseGetKey> packet);
  void lag(uint32_t lag);

  void disconnected();
};

// Define notifies emit when connection status changed
class IConnectionNotify : public QObject {
  Q_OBJECT

public:
  void emitClientConnected(Controller *controller);
  void emitClientDisconnected(Controller *controller);

signals:
  // Emits when a session is initialized as a controller
  void clientConnected(Controller *controller);
  void clientDisconnected(Controller *controller);
};

class AbstractHandler {
public:
  // To use migratePendingSession
  friend AbstractSession;

  AbstractHandler();
  virtual ~AbstractHandler();

  // Pause the acceptation
  virtual void pause() noexcept = 0;
  // Resume the acceptation
  virtual void resume() noexcept = 0;
  // Shutdown the acceptation
  virtual void shutdown() noexcept = 0;
  // Start listening
  virtual bool listen() noexcept = 0;
  // Delete self
  virtual void deleteSelf() noexcept = 0;

  // Get current pending count
  inline size_t getPendingCount() const noexcept {
    return pending_queue.size();
  }
  // Get current client count
  inline size_t getClientCount() const noexcept { return clients.size(); }

  // Get client controller by hwid
  [[nodiscard]] Controller *getClient(const std::string &hwid) const noexcept;
  // Get client controller by pointer
  [[nodiscard]] Controller *getClient(AbstractSession *session) const noexcept;

  IConnectionNotify notifier;

protected:
  // Add session to pending queue
  bool addPendingSession(AbstractSession *session) noexcept;

  // Remove session
  bool removeSession(AbstractSession *session);

private:
  // Migrate pending session to controller or sub channel
  bool migratePendingSession(AbstractSession *session);

  // Connected and initialized clients, own Controller*
  std::unordered_map<std::string /*hwid*/, Controller * /*controller*/> clients;
  // Connected but not initialized queue, own AbstractSession*
  std::vector<AbstractSession * /*session*/> pending_queue;
};

class AbstractSession {
public:
  enum class Status : uint8_t {
    invalid = 0,
    handshaked = 1,
    reversed1 = 2,
    reversed2 = 4,
  };

  enum class Type : uint8_t {
    tcp,
  };

  AbstractSession(AbstractHandler *handler);
  virtual ~AbstractSession();

  // Get remote ip address
  [[nodiscard]] virtual std::string getRemoteAddress() const noexcept = 0;
  // Get remote port
  [[nodiscard]] virtual uint16_t getRemotePort() const noexcept = 0;
  // Get type
  [[nodiscard]] virtual Type getType() const noexcept = 0;
  // Send json packet
  bool sendJsonPacket(const Packet::AbstractGenerator &packet) noexcept;
  // Disconnect from remote
  virtual void shutdown() noexcept = 0;
  // Delete self
  virtual void deleteSelf() noexcept = 0;

  // Get the hwid for this session
  [[nodiscard]] inline std::string getHwid() const noexcept { return hwid; }
  // Get the handshake id for this session
  [[nodiscard]] inline std::string getHandshakeId() const noexcept {
    return handshake_id;
  }
  // Get session's status
  [[nodiscard]] inline Status getStatus() const noexcept { return status; }
  // Check if the session is invalid
  [[nodiscard]] inline bool isInvalid() const noexcept {
    return status == Status::invalid;
  }
  // Check if the session is handshaked
  [[nodiscard]] inline bool isHandshaked() const noexcept {
    return (status & Status::handshaked) == Status::handshaked;
  }
  // Is the session a controller
  [[nodiscard]] inline bool isController() const noexcept {
    return role == Packet::Handshake::Role::controller;
  }
  // Get handler
  [[nodiscard]] AbstractHandler *getHandler() const noexcept { return handler; }
  // Get role
  [[nodiscard]] Packet::Handshake::Role getRole() const noexcept {
    return role;
  }

  // Packet notifier
  IPacketNotify notifier;

protected:
  // Set one status
  inline void setStatus(Status status_) { status = status | status_; }
  // Clear status
  inline void clearStatus(Status status_) { status = Status::invalid; }
  // When received packet
  virtual bool onReceivedPacket(const std::string &buffer);

  virtual bool
  sendJsonPacketInternal(const Packet::AbstractGenerator &packet) noexcept = 0;

  // hwid for the session
  std::string hwid;
  std::string handshake_id;

  // Session's role
  Packet::Handshake::Role role = Packet::Handshake::Role::invalid;

private:
  // Accepted by which handler, only reference
  AbstractHandler *handler = nullptr;

  // Current session status
  Status status = Status::invalid;
};
} // namespace Network