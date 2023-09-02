#include "AApch.h"
#include "LinearCongruentialPseudoRNG.h"

constexpr const uint32_t BRIDGE_VERSION = LCG_RANDOM();

uint32_t getBridgeVersion() { return BRIDGE_VERSION; }