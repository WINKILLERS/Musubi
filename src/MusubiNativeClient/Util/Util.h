#pragma once
#include "Protocols.h"

std::wstring utf8to16(const std::string &u8str);
std::string utf16to8(const std::wstring &u16str);
std::string utf8to8(const std::u8string &u8str);

Packet::ResponseInformation getInformation();
Packet::ResponseGetProcess getProcess();
Packet::ResponseHeartbeat getFocusWindow();
Packet::ResponseQueryProgram queryProgram();