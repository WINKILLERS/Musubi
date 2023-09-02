#pragma once
#include "AbstractChannel.h"
#include "Protocols.h"

namespace Network {
// Forward declare
class AbstractSession;
class Controller;

class InputChannel : public AbstractChannel {
  Q_OBJECT

public:
  InputChannel(AbstractSession *session, AbstractChannel *parent);
  virtual ~InputChannel();

public slots:
  bool sendMouse(
      const std::vector<Packet::RequestSetMouse::MouseInput> &mouse_inputs);
  bool sendKeyboard(const std::vector<Packet::RequestSetKeyboard::KeyBoardInput>
                        &keyboard_inputs);
  void setInputEnabled(bool is_enable) { input_enabled = is_enable; };

  CHANNEL_FORWARD signals :

      private : bool input_enabled = false;
};
} // namespace Network