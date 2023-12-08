#include "Util.hpp"
#include <TlHelp32.h>
#include <bitset>
#include <boost/algorithm/string.hpp>
#include <filesystem>
#include <fmt/format.h>
#include <infoware/infoware.hpp>
#include <shellapi.h>
#include <utf8.h>

#define SECURITY_WIN32
#include <security.h>

namespace Util {
std::wstring utf8to16(const std::string &u8str) {
  std::wstring u16str;
  utf8::utf8to16(u8str.cbegin(), u8str.cend(), std::back_inserter(u16str));
  return u16str;
}

std::string utf16to8(const std::wstring &u16str) {
  std::string u8str;
  utf8::utf16to8(u16str.cbegin(), u16str.cend(), std::back_inserter(u8str));
  return u8str;
}

std::string utf8to8(const std::u8string &u8str) {
  return std::string(u8str.cbegin(), u8str.cend());
}

Bridge::ClientInformation getInformation() {
  Bridge::ClientInformation information;

  information.cpu_model = iware::cpu::model_name();
  information.os_name = iware::system::OS_info().full_name;

  ULONG required_size = 0;
  GetUserNameExW(EXTENDED_NAME_FORMAT::NameSamCompatible, nullptr,
                 &required_size);
  auto *user_buffer = new wchar_t[required_size]{};
  GetUserNameExW(EXTENDED_NAME_FORMAT::NameSamCompatible, user_buffer,
                 &required_size);
  utf8::utf16to8(user_buffer, user_buffer + required_size,
                 std::back_inserter(information.user_name));

  required_size = 0;
  GetComputerNameW(nullptr, &required_size);
  auto *computer_buffer = new wchar_t[required_size]{};
  GetComputerNameW(computer_buffer, &required_size);
  utf8::utf16to8(computer_buffer, computer_buffer + required_size,
                 std::back_inserter(information.computer_name));

  delete[] user_buffer;
  delete[] computer_buffer;

  return information;
}

std::optional<Bridge::ResponseGetProcesses> getProcesses() {
  PROCESSENTRY32W pe32;
  Bridge::ResponseGetProcesses packet;

  auto handle = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);

  if (handle == INVALID_HANDLE_VALUE) {
    return std::nullopt;
  }

  pe32.dwSize = sizeof(PROCESSENTRY32W);
  if (Process32FirstW(handle, &pe32) == false) {
    CloseHandle(handle);
    return std::nullopt;
  }

  do {
    Bridge::Process process;
    process.name = Util::utf16to8(pe32.szExeFile);
    process.pid = pe32.th32ProcessID;
    process.ppid = pe32.th32ParentProcessID;
    packet.addProcess(process);
  } while (Process32NextW(handle, &pe32));

  CloseHandle(handle);

  return packet;
}

std::optional<Bridge::ResponseTerminateProcess> terminateProcess(
    const std::shared_ptr<Bridge::RequestTerminateProcess> packet) {
  const auto &terminating = packet->processes;
  Bridge::ResponseTerminateProcess response;

  for (const auto &pid : terminating) {
    auto handle = OpenProcess(PROCESS_TERMINATE, false, pid);

    if (handle != nullptr) {
      auto success = TerminateProcess(handle, 0);
      response.addProcess(pid, success);
    } else {
      response.addProcess(pid, false);
    }
  }

  return response;
}

std::tuple<std::string, std::string> splitPath(const std::string &path_) {
  std::string path = path_;
  std::string executable;
  std::string parameters;
  size_t exec_end;
  boost::trim(path);

  if (path[0] == '\"') {
    exec_end = path.find_first_of('\"', 1);
    if (std::string::npos != exec_end) {
      executable = path.substr(1, exec_end - 1);
      parameters = path.substr(exec_end + 1);
    } else {
      executable = path.substr(1, exec_end);
    }
  } else {
    exec_end = path.find_first_of(' ', 0);
    if (std::string::npos != exec_end) {
      executable = path.substr(0, exec_end);
      parameters = path.substr(exec_end + 1);
    } else {
      executable = path.substr(0, exec_end);
    }
  }

  return std::make_tuple(executable, parameters);
}

Bridge::ResponseStartProcess
startProcess(const std::shared_ptr<Bridge::RequestStartProcess> packet) {
  Bridge::ResponseStartProcess response;
  const auto &path = packet->path;
  auto [executable, parameters] = splitPath(path);
  uint32_t show_type;

  switch (packet->show) {
  default:
  case Bridge::ShowType::show:
    show_type = SW_SHOW;
    break;
  case Bridge::ShowType::hide:
    show_type = SW_HIDE;
    break;
  case Bridge::ShowType::maximize:
    show_type = SW_MAXIMIZE;
    break;
  case Bridge::ShowType::minimize:
    show_type = SW_MINIMIZE;
    break;
  }

  ShellExecuteW(nullptr, L"open", utf8to16(executable).c_str(),
                utf8to16(parameters).c_str(), nullptr, show_type);

  response.status.path = packet->path;
  response.status.error_code = GetLastError();

  return response;
}

Bridge::ResponseGetFiles
getFiles(const std::shared_ptr<Bridge::RequestGetFiles> packet) {
  Bridge::ResponseGetFiles response;
  const auto &directory = packet->directory;
  std::error_code error_code;

  if (directory.empty()) {
    std::bitset<32> volumes = GetLogicalDrives();
    for (char i = 0; i < volumes.size(); i++) {
      if (volumes[i]) {
        Bridge::File file;
        file.name = fmt::format("{}:\\", (char)('A' + i));
        file.is_directory = true;
        file.size = 0;
        file.last_write_time = 0;
        response.addFile(file);
      }
    }

    response.error_code = GetLastError();
  } else {
    for (const auto &entry : std::filesystem::directory_iterator(
             utf8to16(directory),
             std::filesystem::directory_options::skip_permission_denied |
                 std::filesystem::directory_options::follow_directory_symlink,
             error_code)) {
      Bridge::File file;
      file.name = utf8to8(entry.path().filename().u8string());
      file.is_directory = entry.is_directory();
      file.size = entry.file_size();
      file.last_write_time =
          std::chrono::duration_cast<std::chrono::milliseconds>(
              entry.last_write_time().time_since_epoch())
              .count();
      response.addFile(file);
    }

    response.error_code = error_code.value();
  }

  return response;
}
} // namespace Util
