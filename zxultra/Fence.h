#pragma once

#include "stdafx.h"

namespace zxultra
{

struct Fence
{
    Fence() {};

    Fence(ID3D12Device *device);

    void Flush(ID3D12CommandQueue *commandQueue);

  private:
    UINT64 m_value = 0;
    ComPtr<ID3D12Fence1> m_fence;
};

} // namespace zxultra
