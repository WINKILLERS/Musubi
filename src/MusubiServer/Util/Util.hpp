#ifndef UTIL_HPP
#define UTIL_HPP
#include <string>

namespace Util {
std::wstring utf8to16(const std::string &u8str);
std::string utf16to8(const std::wstring &u16str);
std::string utf8to8(const std::u8string &u8str);
std::u8string utf8to8(const std::string &u8str);
} // namespace Util
#endif
