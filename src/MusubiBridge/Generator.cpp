#include "Generator.h"
#include "AApch.h"

#define UUID_SYSTEM_GENERATOR
#include "crossguid/guid.hpp"

Packet::AbstractGenerator::AbstractGenerator(const Type &type,
                                             const std::string &id,
                                             const uint64_t &timestamp)
    : header(type, id, timestamp) {}

std::string Packet::AbstractGenerator::getId() { return xg::newGuid().str(); }