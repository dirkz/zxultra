#pragma once

#include "stdafx.h"

#include "ConstantBuffer.h"
#include "Constants.h"
#include "CreateFunctions.h"
#include "DescriptorHeap.h"
#include "Fence.h"

namespace zxultra
{

struct FrameData
{
    FrameData() {};

    FrameData(ID3D12Device *device)
        : m_commandAllocator{CreateCommandAllocator(device)}, m_fence{device}, m_cbModel{device, 1},
          m_cbViewProjection{device, 1},
          m_descriptorHeap{device, NumConstantBuffers, D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV,
                           D3D12_DESCRIPTOR_HEAP_FLAG_SHADER_VISIBLE}
    {
        INT index = 0;
        index = m_descriptorHeap.CreateConstantBufferViews(index, m_cbModel);
        index = m_descriptorHeap.CreateConstantBufferViews(index, m_cbViewProjection);
    }

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

    ConstantBuffer<XMFLOAT4X4> m_cbModel;
    ConstantBuffer<XMFLOAT4X4> m_cbViewProjection;

    DescriptorHeap m_descriptorHeap;
};

} // namespace zxultra
