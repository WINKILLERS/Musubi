#pragma once
#include "stdint.h"

enum class ActivationLevel : uint32_t {
  NotActivated,
  Limited,
  Tester,
  Standard,
  Premium
};