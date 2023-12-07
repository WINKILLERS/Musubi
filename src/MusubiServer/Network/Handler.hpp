#ifndef HANDLER_HPP
#define HANDLER_HPP
#include "Session.hpp"
#include <QTcpServer>

namespace Network {
class Handler : public QTcpServer {
  Q_OBJECT;
  friend Session;

public:
  static const uint32_t default_port = 11451;

  Handler(uint32_t port_ = default_port, QObject *parent = nullptr);
  ~Handler();

  inline uint32_t getListeningPort() const { return port; }

  // Get client controller by hwid
  Session *getClient(const std::string &hwid) const;

public slots:
  void pause();
  void resume();
  void shutdown();
  bool run();

signals:
  void clientConnected(Session *client);
  void clientDisconnected(Session *client);

protected:
  void incomingConnection(qintptr socket_descriptor) override;

private:
  bool migratePendingSession(Session *session);

  uint32_t port;

  // Connected and initialized clients
  std::unordered_map<std::string /*hwid*/, Session * /*controller*/> clients;

  // Connected but not initialized sessions
  std::vector<Session * /*session*/> pending_queue;

private slots:
  void handleClientDisconnect();
};
} // namespace Network
#endif