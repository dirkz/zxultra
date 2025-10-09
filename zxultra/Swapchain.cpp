#include "Swapchain.h"

namespace zxultra
{

Swapchain::Swapchain(IDXGIFactory2 *factory, ID3D12Device *device, ID3D12CommandQueue *commandQueue,
                     ID3D12GraphicsCommandList *commandList, HWND hwnd,
                     DescriptorHandleSizes &descriptorHandleSizes)
    : m_descriptorHandleSizes{descriptorHandleSizes}
{
    DXGI_SWAP_CHAIN_DESC1 desc{};
    desc.Format = BackBufferFormat;
    desc.SampleDesc = SampleDescription();
    desc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
    desc.BufferCount = BufferCount;
    desc.SwapEffect = DXGI_SWAP_EFFECT_FLIP_DISCARD;

    HR(factory->CreateSwapChainForHwnd(commandQueue, hwnd, &desc, nullptr, nullptr,
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

    CreateBuffers(device);
    CreateDepthStencilBufferAndView(device, commandList);
}

D3D12_CPU_DESCRIPTOR_HANDLE Swapchain::CurrentBackBufferDescriptorHandle() const
{
    return CD3DX12_CPU_DESCRIPTOR_HANDLE{
        m_rtvDescriptorHeap->GetCPUDescriptorHandleForHeapStart(), m_currentBackBufferIndex,
        m_descriptorHandleSizes.RtvDescriptorHandleIncrementSize()};
}

D3D12_CPU_DESCRIPTOR_HANDLE Swapchain::DepthStencilDescriptorHandle() const
{
    return m_dsvDescriptorHeap->GetCPUDescriptorHandleForHeapStart();
}

DXGI_SAMPLE_DESC Swapchain::SampleDescription() const
{
    DXGI_SAMPLE_DESC sampleDesc{};
    sampleDesc.Count = 1;
    sampleDesc.Quality = 0;

    return sampleDesc;
}

void Swapchain::CreateBuffers(ID3D12Device *device)
{
    CD3DX12_CPU_DESCRIPTOR_HANDLE rtvHeapHandle{
        m_rtvDescriptorHeap->GetCPUDescriptorHandleForHeapStart()};

    for (auto i = 0; i < BufferCount; ++i)
    {
        HR(m_swapchain->GetBuffer(i, IID_PPV_ARGS(m_buffers[i].ReleaseAndGetAddressOf())));
        device->CreateRenderTargetView(m_buffers[i].Get(), nullptr, rtvHeapHandle);
        rtvHeapHandle.Offset(1, m_descriptorHandleSizes.RtvDescriptorHandleIncrementSize());
    }
}

void Swapchain::CreateDepthStencilBufferAndView(ID3D12Device *device,
                                                ID3D12GraphicsCommandList *commandList)
{
    D3D12_RESOURCE_DESC desc{};
    desc.Dimension = D3D12_RESOURCE_DIMENSION_TEXTURE2D;
    desc.Alignment = 0;
    desc.Width = Width();
    desc.Height = Height();
    desc.DepthOrArraySize = 1;
    desc.MipLevels = 1;
    desc.Format = DepthStencilFormat;
    desc.SampleDesc = SampleDescription();
    desc.Layout = D3D12_TEXTURE_LAYOUT_UNKNOWN;
    desc.Flags = D3D12_RESOURCE_FLAG_ALLOW_DEPTH_STENCIL;

    D3D12_CLEAR_VALUE optClear{};
    optClear.Format = DepthStencilFormat;
    optClear.DepthStencil = D3D12_DEPTH_STENCIL_VALUE{1.f, 0};

    auto heapProperties = CD3DX12_HEAP_PROPERTIES{D3D12_HEAP_TYPE_DEFAULT};

    HR(device->CreateCommittedResource(
        &heapProperties, D3D12_HEAP_FLAG_NONE, &desc, D3D12_RESOURCE_STATE_COMMON, &optClear,
        IID_PPV_ARGS(m_depthStencilBuffer.ReleaseAndGetAddressOf())));

    device->CreateDepthStencilView(m_depthStencilBuffer.Get(), nullptr,
                                   DepthStencilDescriptorHandle());

    auto resourceBarrier = CD3DX12_RESOURCE_BARRIER::Transition(
        m_depthStencilBuffer.Get(), D3D12_RESOURCE_STATE_COMMON, D3D12_RESOURCE_STATE_DEPTH_WRITE);

    commandList->ResourceBarrier(1, &resourceBarrier);
}

ID3D12Resource *Swapchain::CurrentBackBufferResource() const
{
    return m_buffers[m_currentBackBufferIndex].Get();
}

void Swapchain::Present()
{
    HR(m_swapchain->Present(0, 0));
    m_currentBackBufferIndex = (m_currentBackBufferIndex + 1) % BufferCount;
}

void Swapchain::Resize(int width, int height)
{
}

} // namespace zxultra