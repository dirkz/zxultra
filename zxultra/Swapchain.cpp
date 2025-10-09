#include "Swapchain.h"

namespace zxultra
{

Swapchain::Swapchain(IDXGIFactory2 *factory, ID3D12Device *device, ID3D12CommandQueue *queue,
                     HWND hwnd)
{
    DXGI_SAMPLE_DESC sampleDesc{};
    sampleDesc.Count = 1;
    sampleDesc.Quality = 0;

    DXGI_SWAP_CHAIN_DESC1 desc{};
    desc.Format = BackBufferFormat;
    desc.SampleDesc = sampleDesc;
    desc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
    desc.BufferCount = BufferCount;
    desc.SwapEffect = DXGI_SWAP_EFFECT_FLIP_DISCARD;

    HR(factory->CreateSwapChainForHwnd(queue, hwnd, &desc, nullptr, nullptr,
                                       m_swapchain.GetAddressOf()));

    HR(m_swapchain->GetDesc1(&desc));
    m_width = desc.Width;
    m_height = desc.Height;

    D3D12_DESCRIPTOR_HEAP_DESC heapDesc{};
    heapDesc.NumDescriptors = BufferCount;
    heapDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_RTV;
    HR(device->CreateDescriptorHeap(&heapDesc, IID_PPV_ARGS(m_rtvDescriptorHeap.GetAddressOf())));

    heapDesc.NumDescriptors = 1;
    heapDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_DSV;
    HR(device->CreateDescriptorHeap(&heapDesc, IID_PPV_ARGS(m_dsvDescriptorHeap.GetAddressOf())));
}

} // namespace zxultra