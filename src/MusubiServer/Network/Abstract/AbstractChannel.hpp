#ifndef ABSTRACT_CHANNEL_HPP
#define ABSTRACT_CHANNEL_HPP
#include "AbstractSession.hpp"
#include "Packet.hpp"
#include <QDialog>
#include <QFuture>
#include <QObject>

namespace Network {
// Forward declare
class AbstractSession;

class AbstractChannel : public QObject {
  Q_OBJECT;

public:
  AbstractChannel(AbstractSession *session, AbstractChannel *parent);
  virtual ~AbstractChannel();

  virtual bool showWindow();

  inline PacketNotifier *getNotifier() { return &session->notifier; }

protected:
  // Own the session
  AbstractSession *session = nullptr;

  // Own the window (if present)
  QDialog *window = nullptr;

  AbstractChannel *parent = nullptr;
};

class AbstractMultiChannel : public AbstractChannel {
  Q_OBJECT;

public:
  AbstractMultiChannel(AbstractSession *session, AbstractChannel *parent)
      : AbstractChannel(session, parent) {}
  virtual ~AbstractMultiChannel();

  // Get sub channel by given role
  AbstractChannel *getSubChannel(Bridge::Role role) const;

  // Request remote to open a channel by given role
  std::optional<QFuture<AbstractChannel *>>
  requestOpenChannel(Bridge::Role role);

  // Add sub channel
  bool addSubChannel(AbstractSession *session);
  // Remove sub channel
  void removeSubChannel(const AbstractSession *session);

signals:
  void channelConnected(Bridge::Role role, AbstractChannel *sub_channel);

protected:
  // Pending channel creation tasks
  std::unordered_map<uint64_t /*id*/, QPromise<AbstractChannel *>>
      pending_tasks;

  // Sub channels, own AbstractChannel*
  std::unordered_map<Bridge::Role, AbstractChannel *> sub_channels;
};
} // namespace Network
#endif