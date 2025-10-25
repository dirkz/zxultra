#include "FrameData.h"

#include "Constants.h"

namespace zxultra
{

FrameData::FrameData(ID3D12Device *device)
    : m_cbProjection{device, 1}, m_cbView{device, 1}, m_cbModel{device, 1},
      m_descriptorHeap{device, NumConstantBuffers, D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV,
                       D3D12_DESCRIPTOR_HEAP_FLAG_SHADER_VISIBLE}
{
    INT index = 0;
    index = m_descriptorHeap.CreateConstantBufferViews(index, m_cbProjection);
    index = m_descriptorHeap.CreateConstantBufferViews(index, m_cbView);
    index = m_descriptorHeap.CreateConstantBufferViews(index, m_cbModel);
}

} // namespace zxultra