#include "TcpHandler.hpp"

namespace Network {
TcpHandler::TcpHandler(uint32_t port_, QObject *parent)
    : port(port_), QTcpServer(parent) {}

void TcpHandler::pause() { resumeAccepting(); }

void TcpHandler::resume() { pauseAccepting(); }

void TcpHandler::shutdown() { close(); }

bool TcpHandler::run() { return QTcpServer::listen(QHostAddress::Any, port); };

void TcpHandler::incomingConnection(qintptr socket_descriptor) {}
} // namespace Network