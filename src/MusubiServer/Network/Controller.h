#pragma once
#include "AbstractChannel.h"
#include "Protocols.h"
#include "qfuture.h"

namespace Network {
// Forward declare
class AbstractHandler;
class AbstractSession;

class Controller : public AbstractMultiChannel {
  Q_OBJECT

public:
  // To use addSubChannel, removeSubChannel
  friend AbstractHandler;

  Controller(Network::AbstractSession *session);
  virtual ~Controller();

  std::string getRemoteAddress() const;
  std::string getHwid() const;
  std::shared_ptr<Packet::ResponseInformation> getInformation() const {
    return information;
  }

  // Request client to send information
  bool requestInformation();

  // Request client to send program
  bool requestProgram();

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

private slots:
  void onInformation(std::shared_ptr<Packet::Header> header,
                     std::shared_ptr<Packet::ResponseInformation> packet);

  void onChannelConnected(Packet::Handshake::Role role,
                          AbstractChannel *sub_channel);

private:
  // The information of the controller
  std::shared_ptr<Packet::ResponseInformation> information;
};
} // namespace Network