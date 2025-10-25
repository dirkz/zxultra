#include "FrameData.h"

namespace zxultra
{

FrameData::FrameData(ID3D12Device *device)
    : m_cbProjection{device, 1}, m_cbView{device, 1}, m_cbModel{device, 1},
      m_descriptorHeap{device, m_cbProjection.Size() + m_cbView.Size() + m_cbModel.Size(),
                       D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV,
                       D3D12_DESCRIPTOR_HEAP_FLAG_SHADER_VISIBLE}
{
    INT index = 0;
    index = m_descriptorHeap.CreateConstantBufferViews(device, index, m_cbProjection);
    index = m_descriptorHeap.CreateConstantBufferViews(device, index, m_cbView);
    index = m_descriptorHeap.CreateConstantBufferViews(device, index, m_cbModel);
}

} // namespace zxultra