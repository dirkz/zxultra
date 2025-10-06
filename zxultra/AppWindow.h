#pragma once

#include <Dxgi.h>
#include <windows.h>
#include <wrl.h>

using Microsoft::WRL::ComPtr;

namespace zxultra
{

struct DXWindow
{
    void OnHwndCreated(HWND hwnd);
    void Resize(int width, int height);

    void Update(double elapsedSeconds);
    void Draw();

    void LogAdapters();
    void LogAdapterOutputs(ComPtr<IDXGIAdapter1> adapter);
};

} // namespace zxultra
