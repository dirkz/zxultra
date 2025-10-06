#pragma once

#include <windows.h>

namespace zxultra
{

struct DXWindow
{
    void OnHwndCreated(HWND hwnd);
    void Update(double elapsedSeconds);
    void Draw();
};

} // namespace zxultra
