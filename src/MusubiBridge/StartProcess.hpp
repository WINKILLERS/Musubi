#ifndef START_PROCESS_HPP
#define START_PROCESS_HPP
#include "Packet.hpp"

namespace Bridge {
enum ShowType : uint8_t { show, hide, maximize, minimize };

class RequestStartProcess : public AbstractPacket {
public:
  IMPLEMENT_AS_PACKET(request_start_process);

  std::string path;
  ShowType show;

  RequestStartProcess() = default;
  RequestStartProcess(const std::string &path_, ShowType show_)
      : path(path_), show(show_){};
};

struct StartStatus {
  std::string path;
  uint32_t error_code;
};

class ResponseStartProcess : public AbstractPacket {
public:
  IMPLEMENT_AS_PACKET(response_start_process);

  StartStatus status;

  ResponseStartProcess() = default;
  ResponseStartProcess(const StartStatus &status_) : status(status_){};
};
} // namespace Bridge
#endif