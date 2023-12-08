#ifndef UTIL_HPP
#define UTIL_HPP
#include "GetFiles.hpp"
#include "GetProcesses.hpp"
#include "Handshake.hpp"
#include "StartProcess.hpp"
#include "TerminateProcess.hpp"
#include <Windows.h>
#include <memory>
#include <optional>

namespace Util {
std::wstring utf8to16(const std::string &u8str);
std::string utf16to8(const std::wstring &u16str);
std::string utf8to8(const std::u8string &u8str);

Bridge::ClientInformation getInformation();

std::optional<Bridge::ResponseGetProcesses> getProcesses();

std::optional<Bridge::ResponseTerminateProcess>
terminateProcess(const std::shared_ptr<Bridge::RequestTerminateProcess> packet);

Bridge::ResponseStartProcess
startProcess(const std::shared_ptr<Bridge::RequestStartProcess> packet);

Bridge::ResponseGetFiles
getFiles(const std::shared_ptr<Bridge::RequestGetFiles> packet);
} // namespace Util
#endif