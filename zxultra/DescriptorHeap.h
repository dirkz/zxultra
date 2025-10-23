#pragma once

#include "stdafx.h"

namespace zxultra
{

struct DescriptorHeap
{
    DescriptorHeap(ID3D12Device *device, UINT numDescriptors,
                   D3D12_DESCRIPTOR_HEAP_TYPE type = D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV,
                   D3D12_DESCRIPTOR_HEAP_FLAGS flags = D3D12_DESCRIPTOR_HEAP_FLAG_SHADER_VISIBLE);

  private:
    UINT m_descriptorHandleIncrementSize;
    ComPtr<ID3D12DescriptorHeap> m_descriptorHeap;
};

} // namespace zxultra
