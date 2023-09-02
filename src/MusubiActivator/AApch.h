#pragma once
#include "algorithm"
#include "argparse/argparse.hpp"
#undef max
#undef min
#include "asio.hpp"
#include "cpr/cpr.h"
#include "fstream"
#include "iostream"
#include "jwt-cpp/jwt.h"
#include "magic_enum.hpp"
#include "nlohmann/json.hpp"
#include "restinio/all.hpp"
#include "restinio/tls.hpp"
#include "spdlog/sinks/basic_file_sink.h"
#include "spdlog/sinks/daily_file_sink.h"
#include "spdlog/spdlog.h"
#include "windows.h"

using namespace std::literals;