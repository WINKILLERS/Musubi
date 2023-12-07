#ifndef UTIL_HPP
#define UTIL_HPP
#include "GetProcesses.hpp"
#include "Handshake.hpp"
#include <Windows.h>
#include <optional>

namespace Util {
std::wstring utf8to16(const std::string &u8str);
std::string utf16to8(const std::wstring &u16str);
std::string utf8to8(const std::u8string &u8str);

Bridge::ClientInformation getInformation();
std::optional<Bridge::ResponseGetProcesses> getProcesses();
} // namespace Util
#endif