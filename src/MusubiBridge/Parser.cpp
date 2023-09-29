#include "Parser.h"
#include "AApch.h"
#include "Protocols.h"

#define CASE_AND_PARSE(pt)                                                     \
  case (Packet::Type)pt::PacketType:                                           \
    body = std::make_shared<pt>();                                             \
    break;

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
      CASE_AND_PARSE(RequestHeartbeatChannel);
      CASE_AND_PARSE(ResponseHeartbeat);
      CASE_AND_PARSE(Handshake);
      CASE_AND_PARSE(RequestInformation);
      CASE_AND_PARSE(ResponseInformation);
      CASE_AND_PARSE(RequestUploadFile);
      CASE_AND_PARSE(ResponseUploadFile);
      CASE_AND_PARSE(RequestDownloadFile);
      CASE_AND_PARSE(ResponseDownloadFile);
      CASE_AND_PARSE(RequestQueryFile);
      CASE_AND_PARSE(ResponseQueryFile);
      CASE_AND_PARSE(RequestDeleteFile);
      CASE_AND_PARSE(ResponseDeleteFile);
      CASE_AND_PARSE(RequestGetProcess);
      CASE_AND_PARSE(ResponseGetProcess);
      CASE_AND_PARSE(RequestTerminateProcess);
      CASE_AND_PARSE(ResponseTerminateProcess);
      CASE_AND_PARSE(RequestStartProcess);
      CASE_AND_PARSE(ResponseStartProcess);
      CASE_AND_PARSE(RequestExecuteFile);
      CASE_AND_PARSE(ResponseExecuteFile);
      CASE_AND_PARSE(RequestScreenChannel);
      CASE_AND_PARSE(RequestRemoteScreenSetArgs);
      CASE_AND_PARSE(ResponseRemoteScreen);
      CASE_AND_PARSE(RequestSetMouse);
      CASE_AND_PARSE(RequestSetKeyboard);
      CASE_AND_PARSE(RequestFileChannel);
      CASE_AND_PARSE(RequestTaskAutoStart);
      CASE_AND_PARSE(ResponseTaskAutoStart);
      CASE_AND_PARSE(Disconnect);
      CASE_AND_PARSE(RequestReinitialize);
      CASE_AND_PARSE(ResponseReinitialize);
      CASE_AND_PARSE(RequestKeyboardMonitorChannel);
      CASE_AND_PARSE(ResponseGetKey);
      CASE_AND_PARSE(RequestQueryProgram);
      CASE_AND_PARSE(ResponseQueryProgram);
      CASE_AND_PARSE(RequestProgramChannel);
      CASE_AND_PARSE(RequestRemoteScreenSync);
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