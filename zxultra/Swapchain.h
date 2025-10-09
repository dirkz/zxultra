#pragma once

#include "stdafx.h"

namespace zxultra
{

constexpr DXGI_FORMAT BackBufferFormat = DXGI_FORMAT_R8G8B8A8_UNORM;
constexpr DXGI_FORMAT DepthStencilFormat = DXGI_FORMAT_D24_UNORM_S8_UINT;
constexpr UINT BufferCount = 2;

struct Swapchain
{
    Swapchain(IDXGIFactory2 *factory, ID3D12Device *device, ID3D12CommandQueue *queue, HWND hwnd);

    UINT Width() const
    {
        return m_width;
    }

    UINT Height() const
    {
        return m_height;
    }

  private:
    ComPtr<IDXGISwapChain1> m_swapchain;

    UINT m_width;
    UINT m_height;

    ComPtr<ID3D12DescriptorHeap> m_rtvDescriptorHeap;
    ComPtr<ID3D12DescriptorHeap> m_dsvDescriptorHeap;
};

} // namespace zxultra
