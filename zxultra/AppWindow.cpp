#include "AppWindow.h"

namespace zxultra
{

DXWindow::DXWindow()
{
}

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

DXWindow::DXWindow(HWND hwnd)
    : m_factory{CreateFactory()}, m_adapter{CreateAdapter(m_factory.Get())},
      m_device{CreateDevice(m_adapter.Get())},
      m_mainCommandQueue{CreateCommandQueue(m_device.Get())}
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
}

void DXWindow::OnHwndCreated(HWND hwnd)
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

    HR(CreateDXGIFactory1(IID_PPV_ARGS(m_factory.GetAddressOf())));
    HR(m_factory->EnumAdapters1(0, m_adapter.GetAddressOf()));

    HR(D3D12CreateDevice(m_adapter.Get(), D3D_FEATURE_LEVEL_11_0,
                         IID_PPV_ARGS(m_device.GetAddressOf())));

    m_descriptorHandleSizes.reset(new DescriptorHandleSizes{m_device.Get()});

    D3D12_COMMAND_QUEUE_DESC queueDesc{};
    queueDesc.Type = D3D12_COMMAND_LIST_TYPE_DIRECT;
    queueDesc.Flags = D3D12_COMMAND_QUEUE_FLAG_NONE;
    HR(m_device->CreateCommandQueue(&queueDesc, IID_PPV_ARGS(m_mainCommandQueue.GetAddressOf())));

    m_frame.reset(new Frame{m_device.Get()});
    m_swapchain.reset(new Swapchain{m_factory.Get(), m_mainCommandQueue.Get(), hwnd});
}

void DXWindow::Resize(int width, int height)
{
}

void DXWindow::Update(double elapsedSeconds)
{
}

void DXWindow::Draw()
{
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
