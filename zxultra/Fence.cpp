#include "Fence.h"

namespace zxultra
{

Fence::Fence(ID3D12Device *device)
{
    HR(device->CreateFence(m_value, D3D12_FENCE_FLAG_NONE, IID_PPV_ARGS(m_fence.GetAddressOf())));
}

} // namespace zxultra