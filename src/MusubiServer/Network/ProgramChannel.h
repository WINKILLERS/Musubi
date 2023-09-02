#pragma once
#include "AbstractChannel.h"
#include "Protocols.h"
#include "qfuture.h"
#include "stack"


namespace Window {
namespace Control {
class ViewProgram;
}
} // namespace Window

namespace Network {
// Forward declare
class AbstractSession;
class Controller;

class ProgramChannel : public AbstractChannel {
  Q_OBJECT

public:
  ProgramChannel(AbstractSession *session, AbstractChannel *parent);
  virtual ~ProgramChannel();

signals:
  void updateCompleted();

  CHANNEL_FORWARD signals
      : void
        recvQueryProgram(std::shared_ptr<Packet::Header> header,
                         std::shared_ptr<Packet::ResponseQueryProgram> packet);

public slots:
  bool refresh();

  bool openInViewFile(const std::wstring &path);

  std::vector<Packet::Program> getPrograms() const noexcept { return programs; }
  size_t getProgramCount() const noexcept { return programs.size(); }
  std::optional<std::string> getCurrentUser() const;

  bool showWindow(QWidget *parent = nullptr) override;

private slots:
  void update(std::shared_ptr<Packet::Header> header,
              std::shared_ptr<Packet::ResponseQueryProgram> packet);

private:
  std::vector<Packet::Program> programs;

  static void sortCaseInsensitive(std::vector<Packet::Program> &programs);
};
} // namespace Network