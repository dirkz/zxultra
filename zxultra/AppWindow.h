#pragma once

#include "stdafx.h"

#include "DescriptorHandleSizes.h"
#include "Fence.h"
#include "Swapchain.h"

namespace zxultra
{

struct DXWindow
{
    DXWindow(HWND hwnd);

    void Resize(int width, int height);

    void Update(double elapsedSeconds);
    void Draw();

    void LogAdapters();
    void LogAdapterOutputs(ComPtr<IDXGIAdapter1> adapter);

  private:
    ComPtr<IDXGIFactory2> m_factory;
    ComPtr<IDXGIAdapter1> m_adapter;
    ComPtr<ID3D12Device> m_device;

    ComPtr<ID3D12CommandQueue> m_commandQueue;
    ComPtr<ID3D12CommandAllocator> m_commandAllocator;
    ComPtr<ID3D12GraphicsCommandList> m_commandList;

    Fence m_fence;

    DescriptorHandleSizes m_descriptorHandleSizes;
    Swapchain m_swapchain;

    D3D12_VIEWPORT m_viewPort{};
    D3D12_RECT m_scissorRect{};
};

} // namespace zxultra
