#include "HeartbeatChannel.h"
#include "AApch.h"
#include "AbstractProtocol.h"


Network::HeartbeatChannel::HeartbeatChannel(AbstractSession *session,
                                            AbstractChannel *parent)
    : AbstractChannel(session, parent) {
  connect(&session->notifier, &IPacketNotify::recvHeartbeat, this,
          &HeartbeatChannel::recvHeartbeat);
}

Network::HeartbeatChannel::~HeartbeatChannel() {}