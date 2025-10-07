#include "Swapchain.h"

namespace zxultra
{

Swapchain::Swapchain(IDXGIFactory2 *factory, UINT sampleCount, UINT qualityLevel)
{
    DXGI_SAMPLE_DESC sampleDesc{};
    sampleDesc.Count = sampleCount;
    sampleDesc.Quality = qualityLevel;

    DXGI_SWAP_CHAIN_DESC1 desc{};
    desc.Format = BackBufferFormat;
    desc.SampleDesc = sampleDesc;
    desc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
    desc.BufferCount = 2;
    desc.SwapEffect = DXGI_SWAP_EFFECT_FLIP_DISCARD;
}

} // namespace zxultra