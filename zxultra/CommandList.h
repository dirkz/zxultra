#pragma once

#include "stdafx.h"

#include "Fence.h"

namespace zxultra
{

struct CommandList
{
    CommandList(ID3D12Device *device, ID3D12CommandQueue *commandQueue);

    /// <summary>
    /// Executes the command list.
    /// </summary>
    void Execute();

    /// <summary>
    /// Flush the command queue.
    /// </summary>
    void Flush();

    /// <summary>
    /// Resets the command list and allocator.
    /// </summary>
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

    inline ID3D12GraphicsCommandList *GetCommandList() const
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
