#pragma once

#include "stdafx.h"

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

  private:
    ComPtr<IDXGIFactory1> m_factory;
    ComPtr<IDXGIAdapter1> m_adapter;
};

} // namespace zxultra
