#pragma once

#include "stdafx.h"

#include "ConstantBuffer.h"
#include "Constants.h"
#include "CreateFunctions.h"
#include "DescriptorHeap.h"
#include "Fence.h"

namespace zxultra
{

/// <summary>
/// Per-frame data
/// </summary>
/// <typeparam name="P">The per-pass object type</typeparam>
/// <typeparam name="O">The per-object object type</typeparam>
/// <typeparam name="I">The number of per-object objects</typeparam>
template <class P, class O, size_t I> struct FrameData
{
    /// <summary>
    /// The number of entries in the root signature.
    /// There are 2 distinct object types (per-pass and per-object),
    /// so this is 2.
    /// </summary>
    /// <returns></returns>
    static const UINT NumRootParameters = 2;

    /// <summary>
    /// The maximum number of per-objects constant buffers.
    /// </summary>
    /// <returns></returns>
    static const UINT NumObjects = I;

    FrameData(ID3D12Device *device)
        : m_commandAllocator{CreateCommandAllocator(device)}, m_fence{device},
          m_cbPerPass{device, 1}, m_cbPerObject{device, I},
          m_descriptorHeap{device, I + 1, D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV,
                           D3D12_DESCRIPTOR_HEAP_FLAG_SHADER_VISIBLE}
    {
        INT index = 0;
        index = m_descriptorHeap.CreateConstantBufferViews(index, m_cbPerPass);
        index = m_descriptorHeap.CreateConstantBufferViews(index, m_cbPerObject);
    }

    FrameData(const FrameData &) = delete;

    FrameData &operator=(const FrameData &) = delete;

    P &PerPass()
    {
        return m_cbPerPass[0];
    }

    O &PerObject(const UINT index)
    {
        return m_cbPerObject[index];
    }

    std::array<ID3D12DescriptorHeap *, 1> DescriptorHeaps()
    {
        return {m_descriptorHeap.Get()};
    }

    DescriptorHeap &GetDescriptorHeap()
    {
        return m_descriptorHeap;
    }

    ID3D12CommandAllocator *CommandAllocator() const
    {
        return m_commandAllocator.Get();
    }

    void FlushFence(ID3D12CommandQueue *commandQueue)
    {
        m_fence.Flush(commandQueue);
    }

  private:
    ComPtr<ID3D12CommandAllocator> m_commandAllocator;

    Fence m_fence;

    ConstantBuffer<P> m_cbPerPass;
    ConstantBuffer<O> m_cbPerObject;

    DescriptorHeap m_descriptorHeap;
};

} // namespace zxultra
