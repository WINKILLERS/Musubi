#ifndef CONTROLLER_HPP
#define CONTROLLER_HPP
#include "../Abstract/AbstractSession.hpp"
#include <QObject>

namespace Network {
class Controller : public QObject {
  Q_OBJECT;

public:
  Controller(AbstractSession *session);

private:
};
} // namespace Network
#endif