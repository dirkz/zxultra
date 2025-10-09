#pragma once

#include "stdafx.h"

#include "DescriptorHandleSizes.h"
#include "Frame.h"
#include "Swapchain.h"

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
    ComPtr<IDXGIFactory2> m_factory;
    ComPtr<IDXGIAdapter1> m_adapter;
    ComPtr<ID3D12Device> m_device;

    std::unique_ptr<DescriptorHandleSizes> m_descriptorHandleSizes;

    ComPtr<ID3D12CommandQueue> m_mainCommandQueue;

    std::unique_ptr<Frame> m_frame;
    std::unique_ptr<Swapchain> m_swapchain;
};

} // namespace zxultra
