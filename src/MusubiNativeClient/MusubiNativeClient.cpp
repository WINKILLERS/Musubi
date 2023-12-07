#include "Network/Controller.hpp"
#include <asio.hpp>
#include <asio/ip/tcp.hpp>
#include <spdlog/spdlog.h>

extern "C" __declspec(dllexport) char host[INET6_ADDRSTRLEN]{"127.0.0.1"};
extern "C" __declspec(dllexport) uint16_t port = 11451;

int main(int argc, char *argv[]) {
  asio::io_context io_context;
  asio::ip::tcp::endpoint endpoint;
  asio::ip::tcp::resolver resolver(io_context);

  spdlog::set_level(spdlog::level::level_enum::trace);

  while (true) {
    try {
      auto result =
          resolver.resolve(asio::ip::tcp::v4(), host, std::to_string(port));
      endpoint = result.begin()->endpoint();
    } catch (const std::exception &e) {
      spdlog::error("error while resolving host, message: {}", e.what());
    }

    auto controller =
        std::make_shared<Network::Controller>(io_context, endpoint);
    if (controller->connect()) {
      controller->run();
    }

    if (controller->isShutdownByServer() == true) {
      break;
    }
  }

  return 0;
}