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
    FrameData() {};

    FrameData(ID3D12Device *device)
        : m_commandAllocator{CreateCommandAllocator(device)}, m_fence{device},
          m_cbPerPass{device, 1}, m_cbPerObject{device, I},
          m_descriptorHeap{device, NumConstantBuffers, D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV,
                           D3D12_DESCRIPTOR_HEAP_FLAG_SHADER_VISIBLE}
    {
        INT index = 0;
        index = m_descriptorHeap.CreateConstantBufferViews(index, m_cbPerPass);
        index = m_descriptorHeap.CreateConstantBufferViews(index, m_cbPerObject);
    }

    FrameData(const FrameData &) = delete;

    FrameData &operator=(const FrameData &) = delete;

    inline FrameData &operator=(FrameData &&other) noexcept
    {
        std::swap(m_commandAllocator, other.m_commandAllocator);
        std::swap(m_fence, other.m_fence);

        m_cbPerPass.Swap(other.m_cbPerPass);
        m_cbPerObject.Swap(other.m_cbPerObject);

        std::swap(m_descriptorHeap, other.m_descriptorHeap);

        return *this;
    }

    void PerPass(const P &data)
    {
        m_cbPerPass[0] = data;
    }

    void PerObject(const UINT index, const O &data)
    {
        m_cbPerObject[index] = data;
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
    // Note: Move semantics!

    ComPtr<ID3D12CommandAllocator> m_commandAllocator;

    Fence m_fence;

    ConstantBuffer<P> m_cbPerPass;
    ConstantBuffer<O> m_cbPerObject;

    DescriptorHeap m_descriptorHeap;
};

} // namespace zxultra
