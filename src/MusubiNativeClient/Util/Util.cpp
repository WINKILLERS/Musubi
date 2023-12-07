#include "Util.hpp"
#include <TlHelp32.h>
#include <infoware/infoware.hpp>
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
} // namespace Util