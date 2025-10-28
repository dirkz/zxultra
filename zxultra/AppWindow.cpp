#include "AppWindow.h"

#include "Blob.h"
#include "Formats.h"

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

AppWindow::AppWindow(HWND hwnd)
    : m_factory{CreateFactory()}, m_adapter{CreateAdapter(m_factory.Get())},
      m_device{CreateDevice(m_adapter.Get())}, m_commandQueue{CreateCommandQueue(m_device.Get())},
      m_fence{m_device.Get()}, m_commandList{m_device.Get()},
      m_descriptorHandleSizes{m_device.Get()},
      m_swapchain{m_factory.Get(),     m_device.Get(), m_commandQueue.Get(),
                  m_commandList.Get(), hwnd,           m_descriptorHandleSizes},
      m_frameData{m_device.Get()}
{
    // sample code for querying features
    CD3DX12FeatureSupport features;
    HR(features.Init(m_device.Get()));
    D3D_FEATURE_LEVEL maxFeatureLevel = features.MaxSupportedFeatureLevel();
    D3D12_RAYTRACING_TIER raytracingTier = features.RaytracingTier();

    // sample code for querying multisampling capabilities
    constexpr UINT SampleCount = 4;
    UINT numQualityLevels = 0;
    HR(features.MultisampleQualityLevels(BackBufferFormat, SampleCount,
                                         D3D12_MULTISAMPLE_QUALITY_LEVELS_FLAG_NONE,
                                         numQualityLevels));

    DefaultBufferCreator uploadBuffers{m_device.Get()};
    CreateVertexBuffers(uploadBuffers);

    // Wait for the swap chain initialization and buffer uploads.
    m_commandList.Execute(m_commandQueue.Get());
    m_fence.Flush(m_commandQueue.Get());

    CreateRootSignature();
    CreatePipelineState();
}

void AppWindow::Resize(int width, int height)
{
    if (width != m_swapchain.Width() || height != m_swapchain.Height())
    {
        m_fence.Flush(m_commandQueue.Get());

        m_commandList.Reset();

        m_swapchain.Resize(width, height, m_commandList.Get());

        m_commandList.Execute(m_commandQueue.Get());
    }
}

void AppWindow::Update(double elapsedSeconds)
{
    XMMATRIX identity = XMMatrixIdentity();

    XMStoreFloat4x4(&m_frameData.CbModel()[0], identity);
    XMStoreFloat4x4(&m_frameData.CbView()[0], identity);
    XMStoreFloat4x4(&m_frameData.CbProjection()[0], identity);
}

void AppWindow::Draw()
{
    m_fence.Flush(m_commandQueue.Get());

    m_commandList.Reset(m_pipelineState.Get());

    D3D12_VIEWPORT viewport = m_swapchain.FullViewport();
    m_commandList->RSSetViewports(1, &viewport);

    D3D12_RECT scissorRect = m_swapchain.FullScissorRect();
    m_commandList->RSSetScissorRects(1, &scissorRect);

    auto transition1 = CD3DX12_RESOURCE_BARRIER::Transition(m_swapchain.CurrentBackBufferResource(),
                                                            D3D12_RESOURCE_STATE_PRESENT,
                                                            D3D12_RESOURCE_STATE_RENDER_TARGET);
    m_commandList->ResourceBarrier(1, &transition1);

    m_commandList->ClearRenderTargetView(m_swapchain.CurrentBackBufferCPUDescriptorHandle(),
                                         Colors::Black, 0, nullptr);

    m_commandList->ClearDepthStencilView(m_swapchain.DepthStencilCPUDescriptorHandle(),
                                         D3D12_CLEAR_FLAG_DEPTH | D3D12_CLEAR_FLAG_STENCIL, 1.f, 0,
                                         0, nullptr);

    D3D12_CPU_DESCRIPTOR_HANDLE backBuffer = m_swapchain.CurrentBackBufferCPUDescriptorHandle();
    D3D12_CPU_DESCRIPTOR_HANDLE depthStencilBuffer = m_swapchain.DepthStencilCPUDescriptorHandle();
    m_commandList->OMSetRenderTargets(1, &backBuffer, true, &depthStencilBuffer);

    auto descriptorHeaps = m_frameData.DescriptorHeaps();
    m_commandList->SetDescriptorHeaps(static_cast<UINT>(descriptorHeaps.size()),
                                      descriptorHeaps.data());

    m_commandList->SetGraphicsRootSignature(m_rootSignature.Get());

    m_commandList->SetGraphicsRootDescriptorTable(
        0, m_frameData.GetDescriptorHeap()->GetGPUDescriptorHandleForHeapStart());

    m_commandList->IASetVertexBuffers(0, 1, &m_vertexBufferView);
    m_commandList->IASetIndexBuffer(&m_indexBufferView);
    m_commandList->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

    m_commandList->DrawIndexedInstanced(m_vertexBuffer.NumIndices(), 1, 0, 0, 0);

    auto transition2 = CD3DX12_RESOURCE_BARRIER::Transition(m_swapchain.CurrentBackBufferResource(),
                                                            D3D12_RESOURCE_STATE_RENDER_TARGET,
                                                            D3D12_RESOURCE_STATE_PRESENT);
    m_commandList->ResourceBarrier(1, &transition2);

    m_commandList.Execute(m_commandQueue.Get());

    m_swapchain.Present();
}

void AppWindow::WillShutdown()
{
    m_fence.Flush(m_commandQueue.Get());
}

void AppWindow::LogAdapters()
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

void AppWindow::LogAdapterOutputs(ComPtr<IDXGIAdapter1> adapter)
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

void AppWindow::CreateVertexBuffers(DefaultBufferCreator &bufferCreator)
{
    VertexWithColor v0{{-0.5f, -0.5f, 0.f}, Colors::Red};
    VertexWithColor v1{{+0.0f, +0.5f, 0.f}, Colors::Green};
    VertexWithColor v2{{+0.5f, -0.5f, 0.f}, Colors::Blue};

    m_vertexBuffer = VertexBuffer<VertexWithColor, IndexType>{v0, v1, v2};

    m_vertexBufferResource =
        bufferCreator.CreateDefaultBuffer(m_commandList.Get(), m_vertexBuffer.Vertices());

    m_vertexBufferView.BufferLocation = m_vertexBufferResource->GetGPUVirtualAddress();
    m_vertexBufferView.SizeInBytes = static_cast<UINT>(m_vertexBuffer.Vertices().size_bytes());
    m_vertexBufferView.StrideInBytes = sizeof(VertexWithColor);

    m_indexBufferResource =
        bufferCreator.CreateDefaultBuffer(m_commandList.Get(), m_vertexBuffer.Indices());

    m_indexBufferView.BufferLocation = m_indexBufferResource->GetGPUVirtualAddress();
    m_indexBufferView.SizeInBytes = static_cast<UINT>(m_vertexBuffer.Indices().size_bytes());
    m_indexBufferView.Format = IndexFormat;
}

void AppWindow::CreateRootSignature()
{
    CD3DX12_ROOT_PARAMETER rootParameters[1]{};
    CD3DX12_DESCRIPTOR_RANGE descriptorRange{};
    constexpr UINT baseShaderRegister = 0;
    descriptorRange.Init(D3D12_DESCRIPTOR_RANGE_TYPE_CBV, NumConstantBuffers, baseShaderRegister);
    rootParameters[0].InitAsDescriptorTable(1, &descriptorRange);

    constexpr UINT numStaticSamples = 0;
    constexpr D3D12_STATIC_SAMPLER_DESC *samplerDescription = nullptr;
    CD3DX12_ROOT_SIGNATURE_DESC rootSignatureDescription{
        _countof(rootParameters), rootParameters, numStaticSamples, samplerDescription,
        D3D12_ROOT_SIGNATURE_FLAG_ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT};

    ComPtr<ID3DBlob> serializedRootSignature = nullptr;
    ComPtr<ID3DBlob> errorBlob = nullptr;
    HRESULT hr = D3D12SerializeRootSignature(
        &rootSignatureDescription, D3D_ROOT_SIGNATURE_VERSION_1,
        serializedRootSignature.GetAddressOf(), errorBlob.GetAddressOf());

    if (!SUCCEEDED(hr) && errorBlob.Get() != nullptr)
    {
        const WCHAR *msg = reinterpret_cast<WCHAR *>(errorBlob->GetBufferPointer());
        OutputDebugString(L"D3D12SerializeRootSignature failed with the following error:\n");
        OutputDebugString(msg);
        OutputDebugString(L"\n");
    }

    HR(hr);

    constexpr UINT nodeMask = 0;
    HR(m_device->CreateRootSignature(nodeMask, serializedRootSignature->GetBufferPointer(),
                                     serializedRootSignature->GetBufferSize(),
                                     IID_PPV_ARGS(m_rootSignature.GetAddressOf())));
}

void AppWindow::CreatePipelineState()
{
    ComPtr<ID3DBlob> vertexShaderBlob = LoadBlob(L"shaders", L"PositionColor_VS.cso");
    ComPtr<ID3DBlob> fragmentShaderBlob = LoadBlob(L"shaders", L"PositionColor_PS.cso");

    D3D12_SHADER_BYTECODE vsByteCode = ByteCode(vertexShaderBlob);
    D3D12_SHADER_BYTECODE psByteCode = ByteCode(fragmentShaderBlob);

    std::vector<D3D12_INPUT_ELEMENT_DESC> inputElementDescriptions =
        VertexWithColor::ElementDescriptions();
    D3D12_INPUT_LAYOUT_DESC inputLayoutDesc{inputElementDescriptions.data(),
                                            static_cast<UINT>(inputElementDescriptions.size())};

    D3D12_GRAPHICS_PIPELINE_STATE_DESC desc{};
    desc.pRootSignature = m_rootSignature.Get();
    desc.VS = vsByteCode;
    desc.PS = psByteCode;
    desc.InputLayout = inputLayoutDesc;
    desc.BlendState = CD3DX12_BLEND_DESC{CD3DX12_DEFAULT{}};
    desc.SampleMask = UINT_MAX;
    desc.RasterizerState = CD3DX12_RASTERIZER_DESC{CD3DX12_DEFAULT{}};
    desc.DepthStencilState = CD3DX12_DEPTH_STENCIL_DESC{CD3DX12_DEFAULT{}};
    desc.PrimitiveTopologyType = D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE;
    desc.NumRenderTargets = 1;
    desc.RTVFormats[0] = BackBufferFormat;
    desc.DSVFormat = DepthStencilFormat;
    desc.SampleDesc = Swapchain::SampleDescription();

    HR(m_device->CreateGraphicsPipelineState(&desc, IID_PPV_ARGS(m_pipelineState.GetAddressOf())));
}

} // namespace zxultra
