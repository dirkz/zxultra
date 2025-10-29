#pragma once

#include "stdafx.h"

#include "ConstantBuffer.h"
#include "DescriptorHeap.h"
#include "Fence.h"

namespace zxultra
{

struct FrameData
{
    FrameData() {};

    FrameData(ID3D12Device *device);

    FrameData(const FrameData &) = delete;

    FrameData &operator=(const FrameData &) = delete;

    inline FrameData &operator=(FrameData &&other) noexcept
    {
        std::swap(m_commandAllocator, other.m_commandAllocator);
        std::swap(m_fence, other.m_fence);

        m_cbModel.Swap(other.m_cbModel);
        m_cbViewProjection.Swap(other.m_cbViewProjection);

        std::swap(m_descriptorHeap, other.m_descriptorHeap);

        return *this;
    }

    ConstantBuffer<XMFLOAT4X4> &CbModel()
    {
        return m_cbModel;
    }

    ConstantBuffer<XMFLOAT4X4> &CbViewProjection()
    {
        return m_cbViewProjection;
    }

    std::array<ID3D12DescriptorHeap *, 1> DescriptorHeaps();

    DescriptorHeap &GetDescriptorHeap()
    {
        return m_descriptorHeap;
    }

    ID3D12CommandAllocator *CommandAllocator() const
    {
        return m_commandAllocator.Get();
    }

    void FlushFence(ID3D12CommandQueue *commandQueue);

  private:
    // Note: Move semantics!

    ComPtr<ID3D12CommandAllocator> m_commandAllocator;

    Fence m_fence;

    ConstantBuffer<XMFLOAT4X4> m_cbModel;
    ConstantBuffer<XMFLOAT4X4> m_cbViewProjection;

    DescriptorHeap m_descriptorHeap;
};

} // namespace zxultra
