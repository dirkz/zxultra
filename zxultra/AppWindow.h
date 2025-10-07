#pragma once

#include "stdafx.h"

#include "Frame.h"

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
    ComPtr<ID3D12Device> m_device;
    ComPtr<ID3D12CommandQueue> m_mainCommandQueue;
    std::unique_ptr<Frame> m_frame;
};

} // namespace zxultra
