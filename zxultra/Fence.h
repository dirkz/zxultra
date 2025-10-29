#pragma once

#include "stdafx.h"

namespace zxultra
{

struct Fence
{
    Fence() {};

    Fence(ID3D12Device *device);

    Fence(const Fence &) = delete;

    Fence(Fence &&other)
        : m_value{std::move(other.m_value)}, m_fence{std::move(other.m_fence)}
    {
        other.m_fence = nullptr;
    }

    Fence &operator=(const Fence &) = delete;

    Fence &operator=(Fence &&other) noexcept
    {
        std::swap(m_value, other.m_value);
        std::swap(m_fence, other.m_fence);

        return *this;
    }

    void Flush(ID3D12CommandQueue *commandQueue);

  private:
    // Note: Move semantics!

    UINT64 m_value = 0;
    ComPtr<ID3D12Fence1> m_fence;
};

} // namespace zxultra
