#include "Program.h"
#include "AApch.h"
#include "Util/Util.h"
#include "bitset"
#include "fstream"
#include "shellapi.h"

Network::Program::~Program() {}

void Network::Program::run() noexcept {
  performHandshake(Packet::Handshake::Role::program);

  registerCallback(Packet::Type::request_query_program, *this,
                   &Network::Program::onQuery);

  while (true) {
    auto parser = readJsonPacket();

    if (parser.has_value() == false) {
      return;
    }

    dispatch(parser.value());
  }
}

bool Network::Program::onQuery(
    std::shared_ptr<Packet::Header> header,
    std::shared_ptr<Packet::AbstractPacket> param) noexcept {
  auto packet = std::dynamic_pointer_cast<Packet::RequestQueryProgram>(param);

  auto generator =
      Packet::Generator<Packet::ResponseQueryProgram>(queryProgram());
  generator.setId(header->id);
  return sendJsonPacket(generator);
}