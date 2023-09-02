#include "AApch.h"
#include "Network/Controller.h"

// extern "C" __declspec(dllexport) char host[INET6_ADDRSTRLEN]{
// "49.232.230.223" };
extern "C" __declspec(dllexport) char host[INET6_ADDRSTRLEN]{"127.0.0.1"};
extern "C" __declspec(dllexport) uint16_t port = 11451;

int main() {
  asio::io_context io_context;
  asio::ip::tcp::endpoint endpoint;
  asio::ip::tcp::resolver resolver(io_context);

  spdlog::set_level(spdlog::level::level_enum::debug);

  while (true) {
    try {
      auto result =
          resolver.resolve(asio::ip::tcp::v4(), host, std::to_string(port));
      endpoint = result.begin()->endpoint();
    } catch (const std::exception &e) {
      spdlog::error("error resolving host, what: {}", e.what());
    }

    std::shared_ptr<Network::AbstractClient> controller =
        std::make_shared<Network::Controller>(io_context, endpoint);
    if (controller->connect()) {
      controller->run();
    }
  }

  return 0;
}