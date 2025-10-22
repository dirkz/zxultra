#pragma once

#include "stdafx.h"

#include "DescriptorHandleSizes.h"
#include "Formats.h"

namespace zxultra
{

constexpr UINT BufferCount = 2;

struct Swapchain
{
    static DXGI_SAMPLE_DESC SampleDescription();

    Swapchain(IDXGIFactory2 *factory, ID3D12Device *device, ID3D12CommandQueue *commandQueue,
              ID3D12GraphicsCommandList *commandList, HWND hwnd,
              DescriptorHandleSizes &descriptorHandleSizes);

    void CreateBuffers(ID3D12Device *device);
    void CreateDepthStencilBufferAndView(ID3D12Device *device,
                                         ID3D12GraphicsCommandList *commandList);

    void Present();
    void Resize(int width, int height, ID3D12Device *device,
                ID3D12GraphicsCommandList *commandList);

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

    ID3D12Resource *CurrentBackBufferResource() const;

  private:
    ComPtr<IDXGISwapChain1> m_swapchain;

    UINT m_width;
    UINT m_height;

    ComPtr<ID3D12DescriptorHeap> m_rtvDescriptorHeap;
    ComPtr<ID3D12DescriptorHeap> m_dsvDescriptorHeap;

    INT m_currentBackBufferIndex = 0;

    DescriptorHandleSizes m_descriptorHandleSizes;

    std::array<ComPtr<ID3D12Resource>, BufferCount> m_buffers;

    ComPtr<ID3D12Resource> m_depthStencilBuffer;
};

} // namespace zxultra
