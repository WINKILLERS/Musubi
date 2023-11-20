#ifndef CONTROLLER_HPP
#define CONTROLLER_HPP
#include "../Abstract/AbstractChannel.hpp"
#include "../Abstract/AbstractSession.hpp"
#include <QObject>

namespace Network {
class Controller : public AbstractMultiChannel {
  Q_OBJECT;

public:
  Controller(AbstractSession *session);
  virtual ~Controller();

private:
};
} // namespace Network
#endif