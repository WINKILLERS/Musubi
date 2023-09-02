#pragma once
#include "Protocols.h"
#include "Tcp.h"


class ScreenCapturer {
public:
  ScreenCapturer(uint16_t bit_count = 16);
  ~ScreenCapturer();

private:
  bool ConstructBI(uint16_t biBitCount, uint32_t biWidth, uint32_t biHeight);

  HWND desktop_hwnd = nullptr;
  HDC desktop_dc = nullptr;
  HDC memory_dc = nullptr;
  uint32_t screen_width = 0;
  uint32_t screen_height = 0;
  LPBITMAPINFO bmi = nullptr;
  void *full_buffer = nullptr;
};

namespace Network {
class Screen : public TcpClient {
public:
  Screen(asio::io_context &io_context, const asio::ip::tcp::endpoint &endpoint,
         const std::string &handshake_id)
      : TcpClient(io_context, endpoint, handshake_id) {}
  virtual ~Screen();

  // AbstractClient implementation
  virtual void run() noexcept override;

private:
};
} // namespace Network