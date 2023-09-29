#pragma once
#include "AbstractChannel.h"
#include "Protocols.h"

namespace Network {
// Forward declare
class AbstractSession;
class Controller;

class ScreenChannel : public AbstractMultiChannel {
  Q_OBJECT

public:
  ScreenChannel(AbstractSession *session, AbstractChannel *parent);
  virtual ~ScreenChannel();

signals:
  void rectChanged(QRect rect, const std::string &buffer);

  CHANNEL_FORWARD signals
      : void
        recvRemoteScreen(std::shared_ptr<Packet::Header> header,
                         std::shared_ptr<Packet::ResponseRemoteScreen> packet);

public slots:
  bool setArgs(uint8_t compression_level = 3);
  bool sendMouse(
      const std::vector<Packet::RequestSetMouse::MouseInput> &mouse_inputs);
  bool sendKeyboard(const std::vector<Packet::RequestSetKeyboard::KeyBoardInput>
                        &keyboard_inputs);
  void setInputEnabled(bool is_enable);
  bool sendSync();

  bool showWindow() noexcept override;

private slots:
  void update(std::shared_ptr<Packet::Header> header,
              std::shared_ptr<Packet::ResponseRemoteScreen> packet);

  void onChannelConnected(Packet::Handshake::Role role,
                          AbstractChannel *sub_channel);

private:
};
} // namespace Network