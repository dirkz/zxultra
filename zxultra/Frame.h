#pragma once

#include "stdafx.h"

#include "Fence.h"

namespace zxultra
{

struct Frame
{
    Frame(ID3D12Device *device);

  private:
    Fence m_fence;
    ComPtr<ID3D12CommandQueue> m_commandQueue;
    ComPtr<ID3D12CommandAllocator> m_commandAllocator;
    ComPtr<ID3D12GraphicsCommandList> m_commandList;
};

} // namespace zxultra
