#include "DescriptorHeap.h"

namespace zxultra
{

DescriptorHeap::DescriptorHeap(ID3D12Device *device, UINT numDescriptors,
                               D3D12_DESCRIPTOR_HEAP_TYPE type, D3D12_DESCRIPTOR_HEAP_FLAGS flags)
    : m_device{device},
      m_descriptorHandleIncrementSize{device->GetDescriptorHandleIncrementSize(type)}
{
    D3D12_DESCRIPTOR_HEAP_DESC desc{};
    desc.NumDescriptors = numDescriptors;
    desc.Type = type;
    desc.Flags = flags;

    HR(device->CreateDescriptorHeap(&desc, IID_PPV_ARGS(m_descriptorHeap.GetAddressOf())));
}

} // namespace zxultra