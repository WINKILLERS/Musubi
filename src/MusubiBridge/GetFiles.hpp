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
  bool is_directory;
  size_t size;
  uint64_t last_write_time;
};

class ResponseGetFiles : public AbstractPacket {
public:
  IMPLEMENT_AS_PACKET(response_get_files);

  int32_t error_code;
  std::vector<File> files;

  inline void addFile(const File &file) { files.emplace_back(file); };

  ResponseGetFiles() = default;
  ResponseGetFiles(const std::vector<File> &files_) : files(files_) {}
};
} // namespace Bridge
#endif
