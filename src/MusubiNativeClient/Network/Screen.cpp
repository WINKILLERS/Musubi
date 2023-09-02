#include "Screen.h"
#include "AApch.h"
#include "Util/Util.h"
#include "experimental/mdspan"

ScreenCapturer::ScreenCapturer(uint16_t bit_count) {
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

  auto bitmap_full = ::CreateDIBSection(memory_dc, bmi, DIB_RGB_COLORS,
                                        &full_buffer, NULL, NULL);

  // std::experimental::mdspan(
  //     full_buffer, std::experimental::extents{screen_width, screen_height});
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

  desktop_hwnd = nullptr;
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
  bmih->biHeight = biHeight;
  bmih->biPlanes = 1;
  bmih->biBitCount = biBitCount;
  bmih->biCompression = BI_RGB;
  bmih->biXPelsPerMeter = 0;
  bmih->biYPelsPerMeter = 0;
  bmih->biClrUsed = 0;
  bmih->biClrImportant = 0;
  bmih->biSizeImage =
      (((bmih->biWidth * bmih->biBitCount + 31) & ~31) >> 3) * bmih->biHeight;

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

Network::Screen::~Screen() {}

void Network::Screen::run() noexcept {
  performHandshake(Packet::Handshake::Role::remote_screen);

  while (true) {
    while (true) {
    }
  }
}