#pragma once

#include "stdafx.h"

#include <VertexBuffer.h>

#include "DefaultBufferCreator.h"
#include "Fence.h"
#include "FrameData.h"
#include "Swapchain.h"
#include "VertexWithColor.h"

namespace zxultra
{

using AppFrameData = FrameData<XMFLOAT4X4, XMFLOAT4X4, 2>;

struct AppWindow
{
    AppWindow(HWND hwnd);

    void Resize(int width, int height);

    void Update(double elapsedSeconds);
    void Draw();

    void WillShutdown();

    void LogAdapters();
    void LogAdapterOutputs(ComPtr<IDXGIAdapter1> adapter);

  private:
    void CreateVertexBuffers(DefaultBufferCreator &bufferCreator);
    void CreateRootSignature();
    void CreatePipelineState();

    inline AppFrameData &CurrentFrameData()
    {
        return m_frameData[m_currentFrameDataIndex];
    }

    ComPtr<IDXGIFactory2> m_factory;
    ComPtr<IDXGIAdapter1> m_adapter;
    ComPtr<ID3D12Device> m_device;
    ComPtr<ID3D12CommandQueue> m_commandQueue;

    /// <summary>
    /// The main fence.
    /// </summary>
    Fence m_fence;

    /// <summary>
    /// The main command allocator.
    /// </summary>
    ComPtr<ID3D12CommandAllocator> m_commandAllocator;

    /// <summary>
    /// The main graphics command list.
    /// </summary>
    ComPtr<ID3D12GraphicsCommandList> m_commandList;

    Swapchain m_swapchain;

    VertexBuffer<VertexWithColor, IndexType> m_vertexBuffer;

    ComPtr<ID3D12Resource> m_vertexBufferResource;
    D3D12_VERTEX_BUFFER_VIEW m_vertexBufferView;

    ComPtr<ID3D12Resource> m_indexBufferResource;
    D3D12_INDEX_BUFFER_VIEW m_indexBufferView;

    std::array<AppFrameData, 3> m_frameData;
    UINT m_currentFrameDataIndex = 0;

    ComPtr<ID3D12RootSignature> m_rootSignature;
    ComPtr<ID3D12PipelineState> m_pipelineState;
};

} // namespace zxultra
