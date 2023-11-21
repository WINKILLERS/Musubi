#include "Util.hpp"
#include <Windows.h>
#include <infoware/infoware.hpp>
#include <utf8.h>

#define SECURITY_WIN32
#include <security.h>

namespace Util {
Bridge::ClientInformation getClientInformation() {
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
} // namespace Util