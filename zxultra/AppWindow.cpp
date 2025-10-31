#include "AppWindow.h"

#include "Blob.h"
#include "CreateFunctions.h"
#include "Formats.h"

namespace zxultra
{

static void Execute(ID3D12GraphicsCommandList *commandList, ID3D12CommandQueue *commandQueue)
{

    HR(commandList->Close());
    ID3D12CommandList *commandLists[]{commandList};
    commandQueue->ExecuteCommandLists(_countof(commandLists), commandLists);
}

AppWindow::AppWindow(HWND hwnd)
    : m_factory{CreateFactory()}, m_adapter{CreateAdapter(m_factory.Get())},
      m_device{CreateDevice(m_adapter.Get())}, m_commandQueue{CreateCommandQueue(m_device.Get())},
      m_fence{m_device.Get()}, m_commandAllocator{CreateCommandAllocator(m_device.Get())},
      m_commandList{CreateGraphicsCommandList(m_device.Get(), m_commandAllocator.Get())},
      m_swapchain{m_factory.Get(), m_device.Get(), m_commandQueue.Get(), m_commandList.Get(), hwnd},
      m_frameData{AppFrameData{m_device.Get()}, AppFrameData{m_device.Get()},
                  AppFrameData{m_device.Get()}}
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
    Execute(m_commandList.Get(), m_commandQueue.Get());
    m_fence.Flush(m_commandQueue.Get());

    m_rootSignature = AppFrameData::CreateRootSignature(m_device.Get());

    CreatePipelineState();
}

void AppWindow::Resize(int width, int height)
{
    if (width != m_swapchain.Width() || height != m_swapchain.Height())
    {
        m_fence.Flush(m_commandQueue.Get());

        HR(m_commandAllocator->Reset());
        m_commandList->Reset(m_commandAllocator.Get(), nullptr);

        m_swapchain.Resize(width, height, m_commandList.Get());

        Execute(m_commandList.Get(), m_commandQueue.Get());
    }
}

void AppWindow::Update(double elapsedSeconds)
{
    float elapsed = static_cast<float>(elapsedSeconds);

    AppFrameData &frameData = CurrentFrameData();

    XMMATRIX projection =
        XMMatrixPerspectiveFovLH(XM_PIDIV4, m_swapchain.AspectRatio(), 0.1f, 10.f);

    XMMATRIX view =
        XMMatrixLookAtLH(XMVectorSet(0, 0, -5.f, 1), XMVectorZero(), XMVectorSet(0, 1.f, 0, 1));

    XMMATRIX viewProjection = XMMatrixMultiply(view, projection);

    float rotationY = elapsed * XM_PI / 10.f;
    float rotationX = std::sin(elapsed * XM_PI / 10.f);

    XMMATRIX model1 = XMMatrixRotationY(rotationY);
    XMMATRIX model2 = XMMatrixRotationX(rotationX);
    XMMATRIX translateLeft = XMMatrixTranslation(-1, 0, 0);
    XMMATRIX modelLeft = XMMatrixMultiply(model1, model2);
    modelLeft = XMMatrixMultiply(modelLeft, translateLeft);

    model1 = XMMatrixRotationY(-rotationY);
    model2 = XMMatrixRotationX(-rotationX);
    XMMATRIX translateRight = XMMatrixTranslation(+1, 0, 0);
    XMMATRIX modelRight = XMMatrixMultiply(model1, model2);
    modelRight = XMMatrixMultiply(modelRight, translateRight);

    XMStoreFloat4x4(&frameData.PerPass(), XMMatrixTranspose(viewProjection));
    XMStoreFloat4x4(&frameData.PerObject(0), XMMatrixTranspose(modelLeft));
    XMStoreFloat4x4(&frameData.PerObject(1), XMMatrixTranspose(modelRight));
}

void AppWindow::Draw()
{
    AppFrameData &frameData = CurrentFrameData();

    frameData.FlushFence(m_commandQueue.Get());

    HR(frameData.CommandAllocator()->Reset());
    HR(m_commandList->Reset(frameData.CommandAllocator(), m_pipelineState.Get()));

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

    m_commandList->IASetVertexBuffers(0, 1, &m_vertexBufferView);
    m_commandList->IASetIndexBuffer(&m_indexBufferView);
    m_commandList->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

    auto descriptorHeaps = frameData.DescriptorHeaps();
    m_commandList->SetDescriptorHeaps(static_cast<UINT>(descriptorHeaps.size()),
                                      descriptorHeaps.data());

    m_commandList->SetGraphicsRootSignature(m_rootSignature.Get());

    // Set per-pass data
    m_commandList->SetGraphicsRootDescriptorTable(1, frameData.PerPassGPUDescriptorHandle());

    // Object 0
    m_commandList->SetGraphicsRootDescriptorTable(0, frameData.PerObjectGPUDescriptorHandle(0));

    m_commandList->DrawIndexedInstanced(m_vertexBuffer.NumIndices(), 1, 0, 0, 0);

    // Object 1
    m_commandList->SetGraphicsRootDescriptorTable(0, frameData.PerObjectGPUDescriptorHandle(1));

    m_commandList->DrawIndexedInstanced(m_vertexBuffer.NumIndices(), 1, 0, 0, 0);

    auto transition2 = CD3DX12_RESOURCE_BARRIER::Transition(m_swapchain.CurrentBackBufferResource(),
                                                            D3D12_RESOURCE_STATE_RENDER_TARGET,
                                                            D3D12_RESOURCE_STATE_PRESENT);
    m_commandList->ResourceBarrier(1, &transition2);

    Execute(m_commandList.Get(), m_commandQueue.Get());

    m_swapchain.Present();

    m_currentFrameDataIndex = (m_currentFrameDataIndex + 1) % m_frameData.size();
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
    VertexWithColor v0{{-0.5f, -0.5f, -0.5f}, Colors::Yellow};    // front left bottom
    VertexWithColor v1{{-0.5f, +0.5f, -0.5f}, Colors::White};     // front left top
    VertexWithColor v2{{+0.5f, +0.5f, -0.5f}, Colors::Cyan};      // front right top
    VertexWithColor v3{{+0.5f, -0.5f, -0.5f}, Colors::Green};     // front right bottom
    VertexWithColor v4{{-0.5f, -0.5f, +0.5f}, Colors::Red};       // back left bottom
    VertexWithColor v5{{-0.5f, +0.5f, +0.5f}, Colors::LightPink}; // back left top
    VertexWithColor v6{{+0.5f, +0.5f, +0.5f}, Colors::Blue};      // back right top
    VertexWithColor v7{{+0.5f, -0.5f, +0.5f}, Colors::Magenta};   // back right bottom

    m_vertexBuffer = VertexBuffer<VertexWithColor, IndexType>{
        v0, v1, v2, // front 1
        v0, v2, v3, // front 2
        v1, v5, v6, // top 1
        v1, v6, v2, // top 2
        v4, v5, v1, // left 1
        v4, v1, v0, // left 2
        v3, v2, v6, // right 1
        v3, v6, v7, // right 2
        v4, v0, v3, // bottom 1
        v4, v3, v7, // bottom 2
        v7, v6, v5, // back 1
        v7, v5, v4, // back 2
    };

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
