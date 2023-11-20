#ifndef ABSTRACT_HANDLER_HPP
#define ABSTRACT_HANDLER_HPP
#include "Common.hpp"
#include <QObject>
#include <memory>
#include <string>
#include <unordered_map>

namespace Network {
// Forward declare
class AbstractSession;
class Controller;

class ClientNotifier final : public QObject {
  Q_OBJECT;

public:
  void emitClientConnected(const Controller *controller);
  void emitClientDisconnected(const Controller *controller);

signals:
  void clientConnected(const Controller *controller);
  void clientDisconnected(const Controller *controller);
};

class AbstractHandler : public IDeleteSelf {
public:
  // To use migratePendingSession, removePendingSession
  friend AbstractSession;

  virtual ~AbstractHandler() = default;

  // Pause the handler
  virtual void pause() = 0;
  // Resume the handler
  virtual void resume() = 0;
  // Shutdown the handler
  virtual void shutdown() = 0;
  // Start handler
  virtual bool run() = 0;

  // Get current pending count
  inline size_t getPendingCount() const { return pending_queue.size(); }
  // Get current client count
  inline size_t getClientCount() const { return clients.size(); }

  // Get client controller by hwid
  Controller *getClient(const std::string &hwid) const;
  // Get client controller by pointer
  Controller *getClient(const AbstractSession *session) const;

  ClientNotifier notifier;

protected:
  // Add session to pending queue
  bool addPendingSession(AbstractSession *session);

private:
  // Remove session
  // TODO: Thread safe
  bool removeSession(const AbstractSession *session /*View only*/);

  // Migrate pending session to its role
  // TODO: Thread safe
  bool migratePendingSession(AbstractSession *session /*View only*/);

  // Connected and initialized clients, own Controller*
  std::unordered_map<std::string /*hwid*/, Controller * /*client*/> clients;
  // Connected but not initialized queue, own AbstractSession*
  std::vector<AbstractSession * /*session*/> pending_queue;
};
} // namespace Network
#endif