#pragma once

#include "stdafx.h"

#include "CommandList.h"
#include "DescriptorHandleSizes.h"
#include "Fence.h"
#include "Swapchain.h"

namespace zxultra
{

struct AppWindow
{
    AppWindow(HWND hwnd);

    void Resize(int width, int height);

    void Update(double elapsedSeconds);
    void Draw();

    void WillShutdown();

    void LogAdapters();
    void LogAdapterOutputs(ComPtr<IDXGIAdapter1> adapter);

  private:
    ComPtr<IDXGIFactory2> m_factory;
    ComPtr<IDXGIAdapter1> m_adapter;
    ComPtr<ID3D12Device> m_device;
    ComPtr<ID3D12CommandQueue> m_commandQueue;

    Fence m_fence;
    CommandList m_commandList;

    DescriptorHandleSizes m_descriptorHandleSizes;
    Swapchain m_swapchain;

    D3D12_VIEWPORT m_viewPort{};
    D3D12_RECT m_scissorRect{};
};

} // namespace zxultra
