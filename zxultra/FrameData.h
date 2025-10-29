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

    inline FrameData& operator=(FrameData&& other) noexcept
    {
        std::swap(m_commandAllocator, other.m_commandAllocator);
        std::swap(m_fence, other.m_fence);

        m_cbProjection = std::move(other.m_cbProjection);
        other.m_cbProjection = ConstantBuffer<XMFLOAT4X4>();

        m_cbView = std::move(other.m_cbView);
        other.m_cbView = ConstantBuffer<XMFLOAT4X4>();

        m_cbModel = std::move(other.m_cbModel);
        other.m_cbModel = ConstantBuffer<XMFLOAT4X4>();

        std::swap(m_descriptorHeap, other.m_descriptorHeap);

        return *this;
    }

    ConstantBuffer<XMFLOAT4X4> &CbProjection()
    {
        return m_cbProjection;
    }

    ConstantBuffer<XMFLOAT4X4> &CbView()
    {
        return m_cbView;
    }

    ConstantBuffer<XMFLOAT4X4> &CbModel()
    {
        return m_cbModel;
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

    ConstantBuffer<XMFLOAT4X4> m_cbProjection;
    ConstantBuffer<XMFLOAT4X4> m_cbView;
    ConstantBuffer<XMFLOAT4X4> m_cbModel;

    DescriptorHeap m_descriptorHeap;
};

} // namespace zxultra
