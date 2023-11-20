#include "Util.h"
#include "AApch.h"
#include "TlHelp32.h"
#include "infoware/infoware.hpp"
#include "windows.h"
#include "winreg/WinReg.hpp"

#define SECURITY_WIN32
#include "security.h"

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

Packet::ResponseInformation getInformation() {
  std::string cpu_model;
  std::string os_name;
  std::string user_name;
  std::string computer_name;

  cpu_model = iware::cpu::model_name();
  os_name = iware::system::OS_info().full_name;

  ULONG required_size = 0;
  GetUserNameExW(EXTENDED_NAME_FORMAT::NameSamCompatible, nullptr,
                 &required_size);
  auto *user_buffer = new wchar_t[required_size]{};
  GetUserNameExW(EXTENDED_NAME_FORMAT::NameSamCompatible, user_buffer,
                 &required_size);
  utf8::utf16to8(user_buffer, user_buffer + required_size,
                 std::back_inserter(user_name));

  required_size = 0;
  GetComputerNameW(nullptr, &required_size);
  auto *computer_buffer = new wchar_t[required_size]{};
  GetComputerNameW(computer_buffer, &required_size);
  utf8::utf16to8(computer_buffer, computer_buffer + required_size,
                 std::back_inserter(computer_name));

  delete[] user_buffer;
  delete[] computer_buffer;

  Packet::ResponseInformation packet(cpu_model, os_name, user_name,
                                     computer_name);
  return packet;
}

Packet::ResponseGetProcess getProcess() {
  std::vector<Packet::Process> list;

  HANDLE snapshot = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);
  PROCESSENTRY32W info{};
  info.dwSize = sizeof(PROCESSENTRY32W);

  while (Process32NextW(snapshot, &info)) {
    std::wstring wname(info.szExeFile);
    std::string name;

    Packet::Process p;
    p.pid = info.th32ProcessID;
    utf8::utf16to8(wname.cbegin(), wname.cend(), std::back_inserter(name));
    p.name = name;
    p.ppid = info.th32ParentProcessID;
    list.push_back(std::move(p));
  }

  CloseHandle(snapshot);

  Packet::ResponseGetProcess packet(list);
  return packet;
}

Packet::ResponseHeartbeat getFocusWindow() {
  WCHAR window_title[512];
  auto focus_window = GetForegroundWindow();
  auto ret = GetWindowTextW(focus_window, window_title, MAX_PATH);
  window_title[ret] = 0;

  Packet::ResponseHeartbeat packet(utf16to8(window_title));
  return packet;
}

Packet::ResponseQueryProgram queryProgram() {
  std::vector<Packet::Program> list;

  winreg::RegKey user_program;
  winreg::RegResult result = user_program.TryOpen(
      HKEY_CURRENT_USER,
      LR"(Software\Microsoft\Windows\CurrentVersion\Uninstall)");
  if (result.IsOk() == true) {
    auto items = user_program.TryEnumSubKeys();
    if (items.IsValid()) {
      for (const auto &v : items.GetValue()) {
        winreg::RegKey item(user_program.Get(), v);
        Packet::Program p;

        auto DisplayName = item.TryGetStringValue(L"DisplayName");
        auto Publisher = item.TryGetStringValue(L"Publisher");
        auto InstallLocation = item.TryGetStringValue(L"InstallLocation");
        auto DisplayVersion = item.TryGetStringValue(L"DisplayVersion");
        auto InstallDate = item.TryGetStringValue(L"InstallDate");

        if (DisplayName.IsValid())
          p.name = utf16to8(DisplayName.GetValue());
        if (Publisher.IsValid())
          p.publisher = utf16to8(Publisher.GetValue());
        if (InstallLocation.IsValid())
          p.install_path = utf16to8(InstallLocation.GetValue());
        if (DisplayVersion.IsValid())
          p.version = utf16to8(DisplayVersion.GetValue());
        if (InstallDate.IsValid())
          p.install_date = utf16to8(InstallDate.GetValue());

        list.push_back(std::move(p));
      }
    }
  }

  winreg::RegKey lm_program;
  result = lm_program.TryOpen(
      HKEY_LOCAL_MACHINE,
      LR"(Software\Microsoft\Windows\CurrentVersion\Uninstall)");
  if (result.IsOk() == true) {
    auto items = lm_program.TryEnumSubKeys();
    if (items.IsValid()) {
      for (const auto &v : items.GetValue()) {
        winreg::RegKey item(lm_program.Get(), v);
        Packet::Program p;

        auto DisplayName = item.TryGetStringValue(L"DisplayName");
        auto Publisher = item.TryGetStringValue(L"Publisher");
        auto InstallLocation = item.TryGetStringValue(L"InstallLocation");
        auto DisplayVersion = item.TryGetStringValue(L"DisplayVersion");
        auto InstallDate = item.TryGetStringValue(L"InstallDate");

        if (DisplayName.IsValid())
          p.name = utf16to8(DisplayName.GetValue());
        if (Publisher.IsValid())
          p.publisher = utf16to8(Publisher.GetValue());
        if (InstallLocation.IsValid())
          p.install_path = utf16to8(InstallLocation.GetValue());
        if (DisplayVersion.IsValid())
          p.version = utf16to8(DisplayVersion.GetValue());
        if (InstallDate.IsValid())
          p.install_date = utf16to8(InstallDate.GetValue());

        if (DisplayName.IsValid() == false && Publisher.IsValid() == false &&
            InstallLocation.IsValid() == false &&
            DisplayVersion.IsValid() == false && InstallDate.IsValid() == false)
          continue;

        list.push_back(std::move(p));
      }
    }
  }

  Packet::ResponseQueryProgram packet(list);
  return packet;
}