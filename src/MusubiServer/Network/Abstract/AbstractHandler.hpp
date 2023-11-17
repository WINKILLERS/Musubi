#ifndef ABSTRACT_HANDLER_HPP
#define ABSTRACT_HANDLER_HPP
#include <string>
#include <unordered_map>

namespace Network {
// Forward declare
class AbstractSession;
class Controller;

class AbstractHandler {
public:
  // Pause the handler
  virtual void pause() = 0;
  // Resume the handler
  virtual void resume() = 0;
  // Shutdown the handler
  virtual void shutdown() = 0;
  // Start handler
  virtual bool run() = 0;

private:
  // Connected and initialized clients, own Controller*
  std::unordered_map<std::string /*hwid*/, Controller * /*client*/> clients;
  // Connected but not initialized queue, own AbstractSession*
  std::vector<AbstractSession * /*session*/> pending_queue;
};
} // namespace Network
#endif