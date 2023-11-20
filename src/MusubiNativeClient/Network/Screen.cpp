#include "Screen.h"
#include "AApch.h"
#include <complex>
#include <optional>

ScreenCapturer::ScreenCapturer(uint16_t bit_count, uint32_t chunk_width,
                               uint32_t chunk_height)
    : chunk_width(chunk_width), chunk_height(chunk_height) {
  // Hwnd of the desktop
  desktop_hwnd = GetDesktopWindow();

  if (desktop_hwnd == nullptr) {
    return;
  }

  // Dc of desktop
  desktop_dc = GetDC(desktop_hwnd);

  if (desktop_dc == nullptr) {
    desktop_hwnd = nullptr;
    return;
  }

  screen_width = GetSystemMetrics(SM_CXSCREEN);
  screen_height = GetSystemMetrics(SM_CYSCREEN);

  // Get memory dc
  memory_dc = CreateCompatibleDC(desktop_dc);

  if (memory_dc == nullptr) {
    ReleaseDC(desktop_hwnd, desktop_dc);
    desktop_dc = nullptr;
    desktop_hwnd = nullptr;
    return;
  }

  auto ret = ConstructBI(bit_count, screen_width, screen_height);

  if (ret == false) {
    DeleteDC(memory_dc);
    ReleaseDC(desktop_hwnd, desktop_dc);
    desktop_dc = nullptr;
    desktop_hwnd = nullptr;
    memory_dc = nullptr;
    return;
  }

  bitmap_full = CreateDIBSection(memory_dc, bmi, DIB_RGB_COLORS, &full_buffer,
                                 NULL, NULL);

  if (SelectObject(memory_dc, bitmap_full) == nullptr) {
    DeleteObject(bitmap_full);
    DeleteDC(memory_dc);
    ReleaseDC(desktop_hwnd, desktop_dc);
    desktop_dc = nullptr;
    desktop_hwnd = nullptr;
    memory_dc = nullptr;
    bitmap_full = nullptr;
    return;
  }

  // Calculate how may chunks we need
  width_chunk_count = screen_width / chunk_width;
  height_chunk_count = screen_height / chunk_height;
  is_width_leftover = screen_width % chunk_width;
  is_height_leftover = screen_height % chunk_height;
  if (is_width_leftover)
    width_chunk_count++;
  if (is_height_leftover)
    height_chunk_count++;
}

ScreenCapturer::~ScreenCapturer() {
  if (bmi != nullptr) {
    delete bmi;
    bmi = nullptr;
  }

  if (memory_dc != nullptr) {
    DeleteDC(memory_dc);
    memory_dc = nullptr;
  }

  if (desktop_dc != nullptr) {
    ReleaseDC(desktop_hwnd, desktop_dc);
    desktop_dc = nullptr;
  }

  if (bitmap_full != nullptr) {
    DeleteObject(bitmap_full);
    bitmap_full = nullptr;
  }

  desktop_hwnd = nullptr;
}

std::optional<std::vector<Packet::ResponseRemoteScreen::ScreenRect>>
ScreenCapturer::getDiff() {
  // If system resolution changed, abort
  if (screen_width != GetSystemMetrics(SM_CXSCREEN) &&
      screen_height != GetSystemMetrics(SM_CYSCREEN)) {
    return std::nullopt;
  }

  std::vector<Packet::ResponseRemoteScreen::ScreenRect> rects;

  BitBlt(memory_dc, 0, 0, screen_width, screen_height, desktop_dc, 0, 0,
         SRCCOPY);

  if (prev_buffers.empty()) {
    // If we are the first time to call the function
    Packet::ResponseRemoteScreen::ScreenRect rect;
    rect.x = 0;
    rect.y = 0;
    rect.width = screen_width;
    rect.height = screen_height;
    rect.screen.resize(bmi->bmiHeader.biSizeImage);
    memcpy(rect.screen.data(), full_buffer, bmi->bmiHeader.biSizeImage);

    // Save current screen
    // Construct buffers
    prev_buffers.resize(width_chunk_count);
    for (auto x = 0; x < width_chunk_count; x++) {
      prev_buffers[x].resize(height_chunk_count, {chunk_width, chunk_height});

      // Reaching leftover
      if (is_width_leftover && x == width_chunk_count - 1) {
        for (auto &y_buffers : prev_buffers[x]) {
          y_buffers.chunk_width = screen_width % chunk_width;
        }
      }

      for (auto y = 0; y < height_chunk_count; y++) {
        // Reaching leftover
        if (is_height_leftover && y == height_chunk_count - 1) {
          prev_buffers[x][y].chunk_height = screen_height % chunk_height;
        }

        // Make hash
        prev_buffers[x][y].hash = getRectHash(x, y);
      }
    }

    rects.push_back(rect);
  } else {
    // We need to compare with last frame
    for (auto x = 0; x < width_chunk_count; x++) {
      for (auto y = 0; y < height_chunk_count; y++) {
        auto hash = getRectHash(x, y);

        // The chunk changed
        const auto &bytesPerPixel = bmi->bmiHeader.biBitCount / 8;
        if (prev_buffers[x][y].hash != hash) {
          Packet::ResponseRemoteScreen::ScreenRect rect;
          rect.x = x * chunk_width;
          rect.y = y * chunk_height;
          rect.width = prev_buffers[x][y].chunk_width;
          rect.height = prev_buffers[x][y].chunk_height;
          rect.screen.resize(rect.width * rect.height * bytesPerPixel);
          copyRectTo(x, y, rect.screen.data());
          rects.push_back(rect);

          // Save again
          prev_buffers[x][y].hash = hash;
        }
      }
    }
  }

  return rects;
}

bool ScreenCapturer::ConstructBI(uint16_t biBitCount, uint32_t biWidth,
                                 uint32_t biHeight) {
  if (bmi != nullptr) {
    delete bmi;
    bmi = nullptr;
  }

  auto color_num = biBitCount <= 8 ? 1 << biBitCount : 0;

  auto bmi_size = sizeof(BITMAPINFOHEADER) + (color_num * sizeof(RGBQUAD));
  bmi = (LPBITMAPINFO) new BYTE[bmi_size];

  BITMAPINFOHEADER *bmih = &bmi->bmiHeader;
  bmih->biSize = sizeof(BITMAPINFOHEADER);
  bmih->biWidth = biWidth;
  // Top-down image
  bmih->biHeight = -biHeight;
  bmih->biPlanes = 1;
  bmih->biBitCount = biBitCount;
  bmih->biCompression = BI_RGB;
  bmih->biXPelsPerMeter = 0;
  bmih->biYPelsPerMeter = 0;
  bmih->biClrUsed = 0;
  bmih->biClrImportant = 0;
  bmih->biSizeImage = (((bmih->biWidth * bmih->biBitCount + 31) & ~31) >> 3) *
                      std::abs(bmih->biHeight);

  if (biBitCount >= 16) {
    return true;
  }

  HDC hDC = GetDC(NULL);
  HBITMAP hBmp = CreateCompatibleBitmap(hDC, 1, 1);

  if (hBmp == nullptr) {
    delete bmi;
    bmi = nullptr;
    return false;
  }

  auto ret = GetDIBits(hDC, hBmp, 0, 0, NULL, bmi, DIB_RGB_COLORS);

  if (ret == 0) {
    delete bmi;
    bmi = nullptr;
    return false;
  }

  ReleaseDC(NULL, hDC);
  DeleteObject(hBmp);

  return true;
}

XXH64_hash_t ScreenCapturer::getRectHash(uint32_t x, uint32_t y) {
  const auto &bytesPerPixel = bmi->bmiHeader.biBitCount / 8;
  auto buffer_size = prev_buffers[x][y].chunk_width *
                     prev_buffers[x][y].chunk_height * bytesPerPixel;
  char *buffer = new char[buffer_size];

  // Copy rect
  copyRectTo(x, y, buffer);

  // Make hash
  auto hash = XXH64(buffer, buffer_size, 0);

  delete[] buffer;

  return hash;
}

void ScreenCapturer::copyRectTo(uint32_t x, uint32_t y, void *dest) {
  // Start copying from original buffer
  const auto &bytesPerPixel = bmi->bmiHeader.biBitCount / 8;
  auto line_scanned = 0;
  auto current_original_pos =
      (char *)full_buffer +
      (y * chunk_height * screen_width + x * chunk_width) * bytesPerPixel;
  auto current_chunk_pos = (char *)dest;
  do {
    memcpy(current_chunk_pos, current_original_pos,
           prev_buffers[x][y].chunk_width * bytesPerPixel);

    current_chunk_pos += prev_buffers[x][y].chunk_width * bytesPerPixel;
    current_original_pos += screen_width * bytesPerPixel;
    line_scanned++;
  } while (line_scanned < prev_buffers[x][y].chunk_height);
}

Network::Screen::~Screen() {}

void Network::Screen::run() noexcept {
  performHandshake(Packet::Handshake::Role::remote_screen);

  while (true) {
    std::optional<Packet::Parser> parser;

    if (sendFrame() == false) {
      return;
    }

    do {
      parser = readJsonPacket();

      if (parser.has_value() == false) {
        return;
      }
    } while (parser->header->type != Packet::Type::request_remote_screen_sync);
  }
}

bool Network::Screen::sendFrame() noexcept {
  auto rects_ret = capturer.getDiff();

  if (rects_ret.has_value() == false) {
    return false;
  }

  auto rects = std::move(rects_ret.value());

  for (auto &rect : rects) {
    auto ret = sendJsonPacket(
        Packet::Generator<Packet::ResponseRemoteScreen>(std::move(rect)));

    if (ret == false) {
      return false;
    }
  }

  return true;
}