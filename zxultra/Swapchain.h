#pragma once

#include "stdafx.h"

#include "Formats.h"

namespace zxultra
{

constexpr UINT BufferCount = 2;

struct Swapchain
{
    static DXGI_SAMPLE_DESC SampleDescription();

    Swapchain(IDXGIFactory2 *factory, ID3D12Device *device, ID3D12CommandQueue *commandQueue,
              ID3D12GraphicsCommandList *commandList, HWND hwnd);

    void CreateBuffers();
    void CreateDepthStencilBufferAndView(ID3D12GraphicsCommandList *commandList);

    void Present();
    void Resize(int width, int height, ID3D12GraphicsCommandList *commandList);

    inline UINT Width() const
    {
        return m_width;
    }

    inline UINT Height() const
    {
        return m_height;
    }

    inline float AspectRatio() const
    {
        return static_cast<float>(m_width) / m_height;
    }

    D3D12_CPU_DESCRIPTOR_HANDLE CurrentBackBufferCPUDescriptorHandle() const;
    D3D12_CPU_DESCRIPTOR_HANDLE DepthStencilCPUDescriptorHandle() const;

    ID3D12Resource *CurrentBackBufferResource() const;

    D3D12_VIEWPORT FullViewport() const;
    D3D12_RECT FullScissorRect() const;

  private:
    ComPtr<ID3D12Device> m_device;
    ComPtr<IDXGISwapChain1> m_swapchain;

    UINT m_rtvDescriptorHandleIncrementSize;

    UINT m_width;
    UINT m_height;

    ComPtr<ID3D12DescriptorHeap> m_rtvDescriptorHeap;
    ComPtr<ID3D12DescriptorHeap> m_dsvDescriptorHeap;

    INT m_currentBackBufferIndex = 0;

    std::array<ComPtr<ID3D12Resource>, BufferCount> m_buffers;

    ComPtr<ID3D12Resource> m_depthStencilBuffer;
};

} // namespace zxultra
