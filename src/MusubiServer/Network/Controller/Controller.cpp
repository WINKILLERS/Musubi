#include "Controller.hpp"

namespace Network {
Controller::Controller(AbstractSession *session)
    : AbstractMultiChannel(session, nullptr) {}
} // namespace Network