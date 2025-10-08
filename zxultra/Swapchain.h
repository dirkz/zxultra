#pragma once

#include "stdafx.h"

namespace zxultra
{

constexpr DXGI_FORMAT BackBufferFormat = DXGI_FORMAT_R8G8B8A8_UNORM;

struct Swapchain
{
    Swapchain(IDXGIFactory2 *factory, ID3D12CommandQueue *queue, HWND hwnd);

  private:
    ComPtr<IDXGISwapChain1> m_swapchain;
};

} // namespace zxultra
