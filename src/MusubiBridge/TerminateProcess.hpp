#ifndef TERMINATE_PROCESS_HPP
#define TERMINATE_PROCESS_HPP
#include "Packet.hpp"
#include <vector>

namespace Bridge {
class RequestTerminateProcess : public AbstractPacket {
public:
  IMPLEMENT_AS_PACKET(request_terminate_process);

  std::vector<uint64_t> processes;

  inline void addProcess(const uint64_t &pid) { processes.emplace_back(pid); };

  RequestTerminateProcess() = default;
  RequestTerminateProcess(const std::vector<uint64_t> &processes_)
      : processes(processes_){};
};

struct TerminateStatus {
  uint64_t pid;
  bool success;
};

class ResponseTerminateProcess : public AbstractPacket {
public:
  IMPLEMENT_AS_PACKET(response_terminate_process);

  std::vector<TerminateStatus> processes;

  inline void addProcess(const uint64_t &pid, bool success) {
    processes.emplace_back(pid, success);
  };

  ResponseTerminateProcess() = default;
  ResponseTerminateProcess(const std::vector<TerminateStatus> &processes_)
      : processes(processes_){};
};
} // namespace Bridge
#endif