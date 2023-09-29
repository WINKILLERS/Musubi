#pragma once
#include "Protocols.h"
#include "qdialog.h"
#include "qfuture.h"

#define CHANNEL_FORWARD

namespace Network {
// Forward declare
class AbstractSession;

class AbstractChannel : public QObject {
  Q_OBJECT

public:
  AbstractChannel(AbstractSession *session, AbstractChannel *parent);
  virtual ~AbstractChannel();

  virtual bool showWindow() noexcept;

signals:
  void lag(uint32_t lag);

protected:
  // Own the session
  AbstractSession *session = nullptr;

  // Own the window (if present)
  QDialog *window = nullptr;

  AbstractChannel *parent = nullptr;
};

class AbstractMultiChannel : public AbstractChannel {
  Q_OBJECT

public:
  AbstractMultiChannel(AbstractSession *session, AbstractChannel *parent)
      : AbstractChannel(session, parent) {}
  virtual ~AbstractMultiChannel();

  // Get sub channel by given role
  AbstractChannel *getSubChannel(Packet::Handshake::Role role) const noexcept;

  // Request remote to open a channel by given role
  std::optional<QFuture<AbstractChannel *>>
  requestOpenChannel(Packet::Handshake::Role role);

signals:
  void channelConnected(Packet::Handshake::Role role,
                        AbstractChannel *sub_channel);

protected:
  // Add sub channel
  bool addSubChannel(AbstractSession *session);
  // Remove sub channel
  void removeSubChannel(AbstractSession *session);

  // Pending channel creation tasks
  std::unordered_map<std::string /*id*/, QPromise<AbstractChannel *>>
      pending_tasks;

  // Sub channels, own AbstractChannel*
  std::unordered_map<Packet::Handshake::Role, AbstractChannel *> sub_channels;
};
} // namespace Network