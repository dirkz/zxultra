#include "FrameData.h"

#include "Constants.h"

namespace zxultra
{

static ComPtr<ID3D12CommandAllocator> CreateCommandAllocator(ID3D12Device *device)
{
    ComPtr<ID3D12CommandAllocator> commandAllocator;

    HR(device->CreateCommandAllocator(D3D12_COMMAND_LIST_TYPE_DIRECT,
                                      IID_PPV_ARGS(commandAllocator.GetAddressOf())));

    return commandAllocator;
}

FrameData::FrameData(ID3D12Device *device)
    : m_commandAllocator{CreateCommandAllocator(device)}, m_cbProjection{device, 1},
      m_cbView{device, 1}, m_cbModel{device, 1},
      m_descriptorHeap{device, NumConstantBuffers, D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV,
                       D3D12_DESCRIPTOR_HEAP_FLAG_SHADER_VISIBLE}
{
    INT index = 0;
    index = m_descriptorHeap.CreateConstantBufferViews(index, m_cbProjection);
    index = m_descriptorHeap.CreateConstantBufferViews(index, m_cbView);
    index = m_descriptorHeap.CreateConstantBufferViews(index, m_cbModel);
}

std::array<ID3D12DescriptorHeap *, 1> FrameData::DescriptorHeaps()
{
    return {m_descriptorHeap.Get()};
}

} // namespace zxultra