#include "AppWindow.h"

namespace zxultra
{

static ComPtr<IDXGIFactory2> CreateFactory()
{

#if defined(_DEBUG)
    // Enable the D3D12 debug layer.
    {

        ComPtr<ID3D12Debug> debugController;
        if (SUCCEEDED(D3D12GetDebugInterface(IID_PPV_ARGS(&debugController))))
        {
            debugController->EnableDebugLayer();
        }
    }
#endif

    ComPtr<IDXGIFactory2> factory;
    HR(CreateDXGIFactory1(IID_PPV_ARGS(factory.GetAddressOf())));

    return factory;
}

static ComPtr<IDXGIAdapter1> CreateAdapter(IDXGIFactory2 *factory)
{
    ComPtr<IDXGIAdapter1> adapter;

    HR(factory->EnumAdapters1(0, adapter.GetAddressOf()));

    return adapter;
}

static ComPtr<ID3D12Device> CreateDevice(IDXGIAdapter *adapter)
{
    ComPtr<ID3D12Device> device;

    HR(D3D12CreateDevice(adapter, D3D_FEATURE_LEVEL_11_0, IID_PPV_ARGS(device.GetAddressOf())));

    return device;
}

static ComPtr<ID3D12CommandQueue> CreateCommandQueue(ID3D12Device *device)
{
    ComPtr<ID3D12CommandQueue> commandQueue;

    D3D12_COMMAND_QUEUE_DESC queueDesc{};
    queueDesc.Type = D3D12_COMMAND_LIST_TYPE_DIRECT;
    queueDesc.Flags = D3D12_COMMAND_QUEUE_FLAG_NONE;
    HR(device->CreateCommandQueue(&queueDesc, IID_PPV_ARGS(commandQueue.GetAddressOf())));

    return commandQueue;
}

static ComPtr<ID3D12CommandAllocator> CreateCommandAllocator(ID3D12Device *device)
{
    ComPtr<ID3D12CommandAllocator> commandAllocator;

    HR(device->CreateCommandAllocator(D3D12_COMMAND_LIST_TYPE_DIRECT,
                                      IID_PPV_ARGS(commandAllocator.GetAddressOf())));

    return commandAllocator;
}

static ComPtr<ID3D12GraphicsCommandList> CreateGraphicsCommandList(
    ID3D12Device *device, ID3D12CommandAllocator *commandAllocator)
{
    ComPtr<ID3D12GraphicsCommandList> commandList;

    HR(device->CreateCommandList(0, D3D12_COMMAND_LIST_TYPE_DIRECT, commandAllocator, nullptr,
                                 IID_PPV_ARGS(commandList.GetAddressOf())));

    return commandList;
}

DXWindow::DXWindow(HWND hwnd)
    : m_factory{CreateFactory()}, m_adapter{CreateAdapter(m_factory.Get())},
      m_device{CreateDevice(m_adapter.Get())}, m_commandQueue{CreateCommandQueue(m_device.Get())},
      m_commandAllocator{CreateCommandAllocator(m_device.Get())},
      m_commandList{CreateGraphicsCommandList(m_device.Get(), m_commandAllocator.Get())},
      m_fence{m_device.Get()}, m_descriptorHandleSizes{m_device.Get()},
      m_swapchain{m_factory.Get(),     m_device.Get(), m_commandQueue.Get(),
                  m_commandList.Get(), hwnd,           m_descriptorHandleSizes}
{
    // sample code for querying features
    CD3DX12FeatureSupport features;
    features.Init(m_device.Get());
    D3D_FEATURE_LEVEL maxFeatureLevel = features.MaxSupportedFeatureLevel();
    D3D12_RAYTRACING_TIER raytracingTier = features.RaytracingTier();

    // sample code for querying multisampling capabilities
    constexpr UINT SampleCount = 4;
    UINT qualityLevels = 0;
    features.MultisampleQualityLevels(BackBufferFormat, SampleCount,
                                      D3D12_MULTISAMPLE_QUALITY_LEVELS_FLAG_NONE, qualityLevels);

    // Wait for the swap chain initialization
    HR(m_commandList->Close());
    ID3D12CommandList *commandLists[]{m_commandList.Get()};
    m_commandQueue->ExecuteCommandLists(_countof(commandLists), commandLists);
    m_fence.SignalAndWait(m_commandQueue.Get());
}

void DXWindow::Resize(int width, int height)
{
}

void DXWindow::Update(double elapsedSeconds)
{
}

void DXWindow::Draw()
{
    HR(m_commandAllocator->Reset());
    HR(m_commandList->Reset(m_commandAllocator.Get(), nullptr));

    auto transition1 = CD3DX12_RESOURCE_BARRIER::Transition(m_swapchain.CurrentBackBufferResource(),
                                                            D3D12_RESOURCE_STATE_PRESENT,
                                                            D3D12_RESOURCE_STATE_RENDER_TARGET);

    m_commandList->ResourceBarrier(1, &transition1);

    D3D12_VIEWPORT viewport{};
    viewport.TopLeftX = 0;
    viewport.TopLeftY = 0;
    viewport.Width = static_cast<FLOAT>(m_swapchain.Width());
    viewport.Height = static_cast<FLOAT>(m_swapchain.Height());
    viewport.MinDepth = 0.f;
    viewport.MaxDepth = 1.f;

    m_commandList->RSSetViewports(1, &viewport);

    D3D12_RECT scissorRect{};
    scissorRect.left = 0;
    scissorRect.right = m_swapchain.Width();
    scissorRect.top = 0;
    scissorRect.bottom = m_swapchain.Height();

    m_commandList->RSSetScissorRects(1, &scissorRect);

    m_commandList->ClearRenderTargetView(m_swapchain.CurrentBackBufferDescriptorHandle(),
                                         DirectX::Colors::CornflowerBlue, 0, nullptr);
    m_commandList->ClearDepthStencilView(m_swapchain.DepthStencilDescriptorHandle(),
                                         D3D12_CLEAR_FLAG_DEPTH | D3D12_CLEAR_FLAG_STENCIL, 1.f, 0,
                                         0, nullptr);

    auto transition2 = CD3DX12_RESOURCE_BARRIER::Transition(m_swapchain.CurrentBackBufferResource(),
                                                            D3D12_RESOURCE_STATE_RENDER_TARGET,
                                                            D3D12_RESOURCE_STATE_PRESENT);

    m_commandList->ResourceBarrier(1, &transition2);

    HR(m_commandList->Close());

    ID3D12CommandList *commandLists[]{m_commandList.Get()};
    m_commandQueue->ExecuteCommandLists(_countof(commandLists), commandLists);
    m_fence.SignalAndWait(m_commandQueue.Get());
}

void DXWindow::LogAdapters()
{
    ComPtr<IDXGIFactory1> factory;
    HR(CreateDXGIFactory1(IID_PPV_ARGS(factory.GetAddressOf())));

    UINT i = 0;
    ComPtr<IDXGIAdapter1> adapter;
    while (factory->EnumAdapters1(i, adapter.ReleaseAndGetAddressOf()) != DXGI_ERROR_NOT_FOUND)
    {
        DXGI_ADAPTER_DESC1 desc;
        HR(adapter->GetDesc1(&desc));

        std::wstring msg = std::format(L"Adapter: {}\n", desc.Description);
        OutputDebugString(msg.c_str());

        LogAdapterOutputs(adapter);

        ++i;
    }
}

void DXWindow::LogAdapterOutputs(ComPtr<IDXGIAdapter1> adapter)
{
    UINT i = 0;
    ComPtr<IDXGIOutput> output;
    while (adapter->EnumOutputs(i, output.ReleaseAndGetAddressOf()) != DXGI_ERROR_NOT_FOUND)
    {
        DXGI_OUTPUT_DESC desc;
        HR(output->GetDesc(&desc));

        std::wstring msg = std::format(L"Output: {}\n", desc.DeviceName);
        OutputDebugString(msg.c_str());

        ++i;
    }
}

} // namespace zxultra
