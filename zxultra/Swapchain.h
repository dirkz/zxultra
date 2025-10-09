#pragma once

#include "stdafx.h"

#include "DescriptorHandleSizes.h"

namespace zxultra
{

constexpr DXGI_FORMAT BackBufferFormat = DXGI_FORMAT_R8G8B8A8_UNORM;
constexpr DXGI_FORMAT DepthStencilFormat = DXGI_FORMAT_D24_UNORM_S8_UINT;
constexpr UINT BufferCount = 2;

struct Swapchain
{
    Swapchain(IDXGIFactory2 *factory, ID3D12Device *device, ID3D12CommandQueue *queue, HWND hwnd,
              DescriptorHandleSizes &descriptorHandleSizes);

    UINT Width() const
    {
        return m_width;
    }

    UINT Height() const
    {
        return m_height;
    }

    D3D12_CPU_DESCRIPTOR_HANDLE CurrentBackBufferDescriptorHandle() const;
    D3D12_CPU_DESCRIPTOR_HANDLE DepthStencilDescriptorHandle() const;

    void CreateBuffers(ID3D12Device *device);

  private:
    ComPtr<IDXGISwapChain1> m_swapchain;

    UINT m_width;
    UINT m_height;

    ComPtr<ID3D12DescriptorHeap> m_rtvDescriptorHeap;
    ComPtr<ID3D12DescriptorHeap> m_dsvDescriptorHeap;

    INT m_currentBackBufferIndex = 0;

    DescriptorHandleSizes m_descriptorHandleSizes;

    std::array<ComPtr<ID3D12Resource>, BufferCount> m_buffers;
};

} // namespace zxultra
