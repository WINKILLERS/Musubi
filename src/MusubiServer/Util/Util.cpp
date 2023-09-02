#include "Util.h"
#include "AApch.h"
#include "utf8cpp/utf8.h"

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

std::u8string utf8to8(const std::string &u8str) {
  return std::u8string(u8str.cbegin(), u8str.cend());
}