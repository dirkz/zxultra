#pragma once

#include "stdafx.h"

#include "Fence.h"

namespace zxultra
{

struct CommandList
{
    CommandList(ID3D12Device *device);

    /// <summary>
    /// Executes the command list on the given command queue.
    /// </summary>
    void Execute(ID3D12CommandQueue *commandQueue);

    /// <summary>
    /// Resets the command list and allocator.
    /// </summary>
    void Reset(ID3D12PipelineState *pipelineState = nullptr);

    inline ID3D12GraphicsCommandList *operator*()
    {
        return m_commandList.Get();
    }

    inline ID3D12GraphicsCommandList *operator->()
    {
        return m_commandList.Get();
    }

    inline ID3D12CommandAllocator *CommandAllocator() const
    {
        return m_commandAllocator.Get();
    }

    inline ID3D12GraphicsCommandList *Get() const
    {
        return m_commandList.Get();
    }

  private:
    ComPtr<ID3D12CommandAllocator> m_commandAllocator;
    ComPtr<ID3D12GraphicsCommandList> m_commandList;
};

} // namespace zxultra
