#pragma once
#include "string"

uint32_t getBridgeVersion();

#define EMPLACE_PARAM(p) packet[#p] = p
#define EXTRACT_PARAM(p) p = packet[#p]

namespace Packet {
enum class Type : uint8_t {
  unknown,
  header,
  response_heartbeat,
  handshake,
  request_disconnect,
  request_information,
  response_information,
  request_upload_file,
  response_upload_file,
  request_download_file,
  response_download_file,
  request_query_file,
  response_query_file,
  request_delete_file,
  response_delete_file,
  request_get_process,
  response_get_process,
  request_terminate_process,
  response_terminate_process,
  request_start_process,
  response_start_process,
  request_execute_file,
  response_execute_file,
  request_remote_screen_set_args,
  response_remote_screen,
  request_set_mouse,
  request_set_keyboard,
  request_file_channel,
  request_task_autostart,
  response_task_autostart,
  request_reinitialize,
  response_reinitialize,
  request_keyboard_monitor_channel,
  response_get_key,
  request_query_program,
  response_query_program,
  request_program_channel,
  request_screen_channel,
  request_heartbeat_channel,
  request_input_channel,
};

class AbstractPacket {
public:
  virtual std::string buildJson() const = 0;
  virtual void parseJson(const std::string &buffer) = 0;

  virtual Type getType() const = 0;
};
} // namespace Packet