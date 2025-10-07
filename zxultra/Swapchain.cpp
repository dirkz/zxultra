#include "Swapchain.h"

namespace zxultra
{

Swapchain::Swapchain(IDXGIFactory2 *factory, ID3D12CommandQueue *queue, HWND hwnd, UINT sampleCount,
                     UINT qualityLevel)
{
    DXGI_SAMPLE_DESC sampleDesc{};
    sampleDesc.Count = 1;
    sampleDesc.Quality = 0;

    DXGI_SWAP_CHAIN_DESC1 desc{};
    desc.Format = BackBufferFormat;
    desc.SampleDesc = sampleDesc;
    desc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
    desc.BufferCount = 2;
    desc.SwapEffect = DXGI_SWAP_EFFECT_FLIP_DISCARD;

    HR(factory->CreateSwapChainForHwnd(queue, hwnd, &desc, nullptr, nullptr,
                                       m_swapchain.GetAddressOf()));

    HR(m_swapchain->GetDesc1(&desc));
}

} // namespace zxultra