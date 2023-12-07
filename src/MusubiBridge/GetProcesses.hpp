#ifndef GET_PROCESSES_HPP
#define GET_PROCESSES_HPP
#include "Packet.hpp"
#include <string>
#include <vector>

namespace Bridge {
class RequestGetProcesses : public AbstractPacket {
public:
  IMPLEMENT_AS_PACKET_DEFAULT(request_get_processes);

  RequestGetProcesses() = default;
};

struct Process {
  std::string name;
  uint64_t pid;
  uint64_t ppid;
};

class ResponseGetProcesses : public AbstractPacket {
public:
  IMPLEMENT_AS_PACKET(response_get_processes);

  std::vector<Process> processes;

  inline void addProcess(const Process &process) {
    processes.emplace_back(process);
  };

  ResponseGetProcesses() = default;
  ResponseGetProcesses(const std::vector<Process> &processes_)
      : processes(processes_){};
};
} // namespace Bridge
#endif