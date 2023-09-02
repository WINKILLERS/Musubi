#include "Parser.h"
#include "AApch.h"
#include "Protocols.h"

Packet::Parser::~Parser() {}

bool Packet::Parser::parseJson(const std::string &buffer) {
  if (buffer.size() <= 2 * sizeof(uint64_t))
    return false;

  auto header_size = *(const uint64_t *)buffer.data();
  auto body_size = *((const uint64_t *)buffer.data() + 1);

  if (buffer.size() < 2 * sizeof(uint64_t) + header_size + body_size)
    return false;

  std::string header_buffer(buffer.data() + 2 * sizeof(uint64_t),
                            buffer.data() + 2 * sizeof(uint64_t) + header_size);
  std::string body_buffer(buffer.data() + 2 * sizeof(uint64_t) + header_size,
                          buffer.data() + 2 * sizeof(uint64_t) + header_size +
                              body_size);

  return parseJson(header_buffer, body_buffer);
}

bool Packet::Parser::parseJson(const std::string &header_buffer,
                               const std::string &body_buffer) {
  if (header == nullptr) {
    header = std::make_shared<Header>();
  }

  header->parseJson(header_buffer);

  if (header->version != getBridgeVersion()) {
    return false;
  }

  if (body == nullptr)
    switch (header->type) {
    case Type::request_heartbeat_channel:
      body = std::make_shared<RequestHeartbeatChannel>();
      break;
    case Type::response_heartbeat:
      body = std::make_shared<ResponseHeartbeat>();
      break;
    case Type::handshake:
      body = std::make_shared<Handshake>();
      break;
    case Type::request_information:
      body = std::make_shared<RequestInformation>();
      break;
    case Type::response_information:
      body = std::make_shared<ResponseInformation>();
      break;
    case Type::request_upload_file:
      body = std::make_shared<RequestUploadFile>();
      break;
    case Type::response_upload_file:
      body = std::make_shared<ResponseUploadFile>();
      break;
    case Type::request_download_file:
      body = std::make_shared<RequestDownloadFile>();
      break;
    case Type::response_download_file:
      body = std::make_shared<ResponseDownloadFile>();
      break;
    case Type::request_query_file:
      body = std::make_shared<RequestQueryFile>();
      break;
    case Type::response_query_file:
      body = std::make_shared<ResponseQueryFile>();
      break;
    case Type::request_delete_file:
      body = std::make_shared<RequestDeleteFile>();
      break;
    case Type::response_delete_file:
      body = std::make_shared<ResponseDeleteFile>();
      break;
    case Type::request_get_process:
      body = std::make_shared<RequestGetProcess>();
      break;
    case Type::response_get_process:
      body = std::make_shared<ResponseGetProcess>();
      break;
    case Type::request_terminate_process:
      body = std::make_shared<RequestTerminateProcess>();
      break;
    case Type::response_terminate_process:
      body = std::make_shared<ResponseTerminateProcess>();
      break;
    case Type::request_start_process:
      body = std::make_shared<RequestStartProcess>();
      break;
    case Type::response_start_process:
      body = std::make_shared<ResponseStartProcess>();
      break;
    case Type::request_execute_file:
      body = std::make_shared<RequestExecuteFile>();
      break;
    case Type::response_execute_file:
      body = std::make_shared<ResponseExecuteFile>();
      break;
    case Type::request_screen_channel:
      body = std::make_shared<RequestScreenChannel>();
      break;
    case Type::request_remote_screen_set_args:
      body = std::make_shared<RequestRemoteScreenSetArgs>();
      break;
    case Type::response_remote_screen:
      body = std::make_shared<ResponseRemoteScreen>();
      break;
    case Type::request_set_mouse:
      body = std::make_shared<RequestSetMouse>();
      break;
    case Type::request_set_keyboard:
      body = std::make_shared<RequestSetKeyboard>();
      break;
    case Type::request_file_channel:
      body = std::make_shared<RequestFileChannel>();
      break;
    case Type::request_task_autostart:
      body = std::make_shared<RequestTaskAutoStart>();
      break;
    case Type::response_task_autostart:
      body = std::make_shared<ResponseTaskAutoStart>();
      break;
    case Type::request_disconnect:
      body = std::make_shared<Disconnect>();
      break;
    case Type::request_reinitialize:
      body = std::make_shared<RequestReinitialize>();
      break;
    case Type::response_reinitialize:
      body = std::make_shared<ResponseReinitialize>();
      break;
    case Type::request_keyboard_monitor_channel:
      body = std::make_shared<RequestKeyboardMonitorChannel>();
      break;
    case Type::response_get_key:
      body = std::make_shared<ResponseGetKey>();
      break;
    case Type::request_query_program:
      body = std::make_shared<RequestQueryProgram>();
      break;
    case Type::response_query_program:
      body = std::make_shared<ResponseQueryProgram>();
      break;
    case Type::request_program_channel:
      body = std::make_shared<RequestProgramChannel>();
      break;
    case Type::header:
    case Type::unknown:
    default:
      return false;
    }

  try {
    body->parseJson(body_buffer);
  } catch (const std::exception &) {
    return false;
  }

  return true;
}