#include "Generator.h"
#include "AApch.h"
#include "crossguid/guid.hpp"

Packet::AbstractGenerator::AbstractGenerator(const Type &type,
                                             const std::string &id,
                                             const uint64_t &timestamp)
    : header(type, id, timestamp) {}

std::string Packet::AbstractGenerator::constructId() {
  return xg::newGuid().str();
}