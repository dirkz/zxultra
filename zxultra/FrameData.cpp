#include "FrameData.h"

#include "Constants.h"
#include "CreateFunctions.h"

namespace zxultra
{

FrameData::FrameData(ID3D12Device *device)
    : m_commandAllocator{CreateCommandAllocator(device)}, m_fence{device}, m_cbModel{device, 1},
      m_cbView{device, 1}, m_cbProjection{device, 1},
      m_descriptorHeap{device, NumConstantBuffers, D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV,
                       D3D12_DESCRIPTOR_HEAP_FLAG_SHADER_VISIBLE}
{
    INT index = 0;
    index = m_descriptorHeap.CreateConstantBufferViews(index, m_cbModel);
    index = m_descriptorHeap.CreateConstantBufferViews(index, m_cbView);
    index = m_descriptorHeap.CreateConstantBufferViews(index, m_cbProjection);
}

std::array<ID3D12DescriptorHeap *, 1> FrameData::DescriptorHeaps()
{
    return {m_descriptorHeap.Get()};
}

void FrameData::FlushFence(ID3D12CommandQueue *commandQueue)
{
    m_fence.Flush(commandQueue);
}

} // namespace zxultra