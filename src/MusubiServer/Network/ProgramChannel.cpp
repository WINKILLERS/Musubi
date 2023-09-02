#include "ProgramChannel.h"
#include "AApch.h"
#include "AbstractProtocol.h"
#include "Network/Controller.h"
#include "Network/FileChannel.h"
#include "Util/Util.h"
#include "Window/Control/Program/ViewProgram.h"
#include "filesystem"

Network::ProgramChannel::ProgramChannel(AbstractSession *session,
                                        AbstractChannel *parent)
    : AbstractChannel(session, parent) {
  connect(&session->notifier, &IPacketNotify::recvQueryProgram, this,
          &ProgramChannel::recvQueryProgram);

  // Connect private slots
  connect(&session->notifier, &IPacketNotify::recvQueryProgram, this,
          &ProgramChannel::update);
}

Network::ProgramChannel::~ProgramChannel() {}

bool Network::ProgramChannel::refresh() {
  return session->sendJsonPacket(
      Packet::Generator<Packet::RequestQueryProgram>());
}

bool Network::ProgramChannel::openInViewFile(const std::wstring &path) {
  auto controller = (Controller *)parent;
  auto request_ret =
      controller->requestOpenChannel(Packet::Handshake::Role::file);
  if (request_ret.has_value() == false) {
    return false;
  }

  auto request = std::move(request_ret.value());
  request.then([this, path](Network::AbstractChannel *sub_channel) {
    auto *file_channel = qobject_cast<Network::FileChannel *>(sub_channel);

    file_channel->openAbsolute(path);
    file_channel->showWindow();
  });

  return true;
}

std::optional<std::string> Network::ProgramChannel::getCurrentUser() const {
  auto controller = (Controller *)parent;
  auto information = controller->getInformation();

  if (information == nullptr) {
    return std::nullopt;
  }

  return information->user_name;
}

bool Network::ProgramChannel::showWindow(QWidget *parent) {
  if (window == nullptr) {
    window = new Window::Control::ViewProgram(this, parent);
  }

  return AbstractChannel::showWindow(parent);
}

void Network::ProgramChannel::update(
    std::shared_ptr<Packet::Header> header,
    std::shared_ptr<Packet::ResponseQueryProgram> packet) {
  spdlog::debug("received response, updating program list");

  programs = packet->list;
  sortCaseInsensitive(programs);

  emit updateCompleted();
}

void Network::ProgramChannel::sortCaseInsensitive(
    std::vector<Packet::Program> &programs) {
  std::sort(programs.begin(), programs.end(),
            [](Packet::Program &p1, Packet::Program &p2) {
              return std::lexicographical_compare(
                  begin(p1.name), end(p1.name), begin(p2.name), end(p2.name),
                  [](const char &char1, const char &char2) {
                    return tolower(char1) < tolower(char2);
                  });
            });
}