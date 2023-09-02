#pragma once
#include "AbstractChannel.h"
#include "Protocols.h"

namespace Network {
// Forward declare
class AbstractSession;
class Controller;

class HeartbeatChannel : public AbstractChannel {
  Q_OBJECT

public:
  HeartbeatChannel(AbstractSession *session, AbstractChannel *parent);
  virtual ~HeartbeatChannel();

  CHANNEL_FORWARD signals
      : void
        recvHeartbeat(std::shared_ptr<Packet::Header> header,
                      std::shared_ptr<Packet::ResponseHeartbeat> packet);

private:
};
} // namespace Network