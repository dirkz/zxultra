#pragma once

#include "stdafx.h"

#include "Fence.h"

namespace zxultra
{

struct GraphicsQueue
{
    GraphicsQueue(ID3D12Device *device);

    void Execute();
    void Flush();
    void Reset();

    ID3D12GraphicsCommandList *operator->();

    inline ID3D12CommandQueue *CommandQueue() const
    {
        return m_commandQueue.Get();
    }

    inline ID3D12CommandAllocator *CommandAllocator() const
    {
        return m_commandAllocator.Get();
    }

    inline ID3D12GraphicsCommandList *CommandList() const
    {
        return m_commandList.Get();
    }

  private:
    ComPtr<ID3D12CommandQueue> m_commandQueue;
    ComPtr<ID3D12CommandAllocator> m_commandAllocator;
    ComPtr<ID3D12GraphicsCommandList> m_commandList;
    Fence m_fence;
};

} // namespace zxultra
