#ifndef REMOVE_FILES_HPP
#define REMOVE_FILES_HPP
#include "Packet.hpp"
#include <string>
#include <vector>

namespace Bridge {
class RequestRemoveFiles : public AbstractPacket {
public:
  IMPLEMENT_AS_PACKET(request_remove_files);

  std::vector<std::string> paths;

  inline void addPath(const std::string &path) { paths.emplace_back(path); };

  RequestRemoveFiles() = default;
  RequestRemoveFiles(const std::vector<std::string> &paths_) : paths(paths_) {}
};

struct RemoveStatus {
  std::string path;
  int32_t error_code;
};

class ResponseRemoveFiles : public AbstractPacket {
public:
  IMPLEMENT_AS_PACKET(response_remove_files);

  std::vector<RemoveStatus> statuses;

  inline void addStatus(const RemoveStatus &status) {
    statuses.emplace_back(status);
  };

  ResponseRemoveFiles() = default;
  ResponseRemoveFiles(const std::vector<RemoveStatus> &statuses_)
      : statuses(statuses_) {}
};
} // namespace Bridge
#endif
