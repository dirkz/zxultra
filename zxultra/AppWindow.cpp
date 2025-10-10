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

DXWindow::DXWindow(HWND hwnd)
    : m_factory{CreateFactory()}, m_adapter{CreateAdapter(m_factory.Get())},
      m_device{CreateDevice(m_adapter.Get())}, m_graphicsQueue{m_device.Get()},
      m_descriptorHandleSizes{m_device.Get()}, m_swapchain{m_factory.Get(),
                                                           m_device.Get(),
                                                           m_graphicsQueue.CommandQueue(),
                                                           m_graphicsQueue.CommandList(),
                                                           hwnd,
                                                           m_descriptorHandleSizes}
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
    m_graphicsQueue.Execute();
    m_graphicsQueue.Flush();
}

void DXWindow::Resize(int width, int height)
{
    if (width != m_swapchain.Width() || height != m_swapchain.Height())
    {
        m_graphicsQueue.Flush();

        m_viewPort.TopLeftX = 0;
        m_viewPort.TopLeftY = 0;
        m_viewPort.Width = static_cast<FLOAT>(width);
        m_viewPort.Height = static_cast<FLOAT>(height);
        m_viewPort.MinDepth = 0.f;
        m_viewPort.MaxDepth = 1.f;

        m_scissorRect.left = 0;
        m_scissorRect.right = width;
        m_scissorRect.top = 0;
        m_scissorRect.bottom = height;

        m_graphicsQueue.Reset();

        m_swapchain.Resize(width, height, m_device.Get(), m_graphicsQueue.CommandList());

        m_graphicsQueue.Execute();
    }
}

void DXWindow::Update(double elapsedSeconds)
{
}

void DXWindow::Draw()
{
    m_graphicsQueue.Flush();

    m_graphicsQueue.Reset();

    auto transition1 = CD3DX12_RESOURCE_BARRIER::Transition(m_swapchain.CurrentBackBufferResource(),
                                                            D3D12_RESOURCE_STATE_PRESENT,
                                                            D3D12_RESOURCE_STATE_RENDER_TARGET);
    m_graphicsQueue->ResourceBarrier(1, &transition1);

    m_graphicsQueue->RSSetViewports(1, &m_viewPort);
    m_graphicsQueue->RSSetScissorRects(1, &m_scissorRect);

    m_graphicsQueue->ClearRenderTargetView(m_swapchain.CurrentBackBufferDescriptorHandle(),
                                           DirectX::Colors::CornflowerBlue, 0, nullptr);
    m_graphicsQueue->ClearDepthStencilView(m_swapchain.DepthStencilDescriptorHandle(),
                                           D3D12_CLEAR_FLAG_DEPTH | D3D12_CLEAR_FLAG_STENCIL, 1.f,
                                           0, 0, nullptr);

    auto transition2 = CD3DX12_RESOURCE_BARRIER::Transition(m_swapchain.CurrentBackBufferResource(),
                                                            D3D12_RESOURCE_STATE_RENDER_TARGET,
                                                            D3D12_RESOURCE_STATE_PRESENT);
    m_graphicsQueue->ResourceBarrier(1, &transition2);

    m_graphicsQueue.Execute();

    m_swapchain.Present();
}

void DXWindow::WillShutdown()
{
    m_graphicsQueue.Flush();
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
