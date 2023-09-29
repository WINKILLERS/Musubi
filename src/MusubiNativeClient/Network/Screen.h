#pragma once
#include "Protocols.h"
#include "Tcp.h"
#include "Util/Util.h"
#include "vector"
#include "windef.h"
#include "xxhash.h"
#include <optional>
#include <stdint.h>

class ScreenCapturer {
public:
  ScreenCapturer(uint16_t bit_count = 16, uint32_t chunk_width = 480,
                 uint32_t chunk_height = 360);
  ~ScreenCapturer();

  std::optional<std::vector<Packet::ResponseRemoteScreen::ScreenRect>>
  getDiff();

private:
  bool ConstructBI(uint16_t biBitCount, uint32_t biWidth, uint32_t biHeight);
  XXH64_hash_t getRectHash(uint32_t x, uint32_t y);
  void copyRectTo(uint32_t x, uint32_t y, void *dest);

  struct Chunk {
    uint32_t chunk_width;
    uint32_t chunk_height;
    XXH64_hash_t hash;
  };

  HWND desktop_hwnd = nullptr;
  HDC desktop_dc = nullptr;
  HDC memory_dc = nullptr;
  uint32_t screen_width = 0;
  uint32_t screen_height = 0;
  LPBITMAPINFO bmi = nullptr;
  HBITMAP bitmap_full = nullptr;
  void *full_buffer = nullptr;
  std::vector<std::vector<Chunk>> prev_buffers;

  uint32_t chunk_width;
  uint32_t chunk_height;
  uint32_t width_chunk_count = 0;
  uint32_t height_chunk_count = 0;
  bool is_width_leftover = false;
  bool is_height_leftover = false;
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
  bool sendFrame() noexcept;

  ScreenCapturer capturer;
};
} // namespace Network