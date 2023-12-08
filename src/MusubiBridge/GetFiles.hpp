#ifndef GET_FILES_HPP
#define GET_FILES_HPP
#include "Packet.hpp"
#include <string>
#include <vector>

namespace Bridge {
class RequestGetFiles : public AbstractPacket {
public:
  IMPLEMENT_AS_PACKET(request_get_files);

  std::string directory;

  RequestGetFiles() = default;
  RequestGetFiles(const std::string &directory_) : directory(directory_) {}
};

struct File {
  std::string name;
};

class ResponseGetFiles : public AbstractPacket {
public:
  IMPLEMENT_AS_PACKET(response_get_processes);

  std::vector<File> files;

  ResponseGetFiles() = default;
  ResponseGetFiles(const std::vector<File> &files_) : files(files_) {}
};
} // namespace Bridge
#endif