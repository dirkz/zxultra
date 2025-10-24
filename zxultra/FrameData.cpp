#include "FrameData.h"

namespace zxultra
{

FrameData::FrameData(ID3D12Device *device)
    : m_cbProjection{device, 1}, m_cbView{device, 1}, m_cbModel{device, 1}
{
    D3D12_DESCRIPTOR_HEAP_DESC desc{};
    desc.NumDescriptors = m_cbProjection.Size() + m_cbView.Size() + m_cbModel.Size();
    desc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV;
    desc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_SHADER_VISIBLE;

    HR(device->CreateDescriptorHeap(&desc, IID_PPV_ARGS(m_descriptorHeap.GetAddressOf())));

    UINT incrementSize =
        device->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV);

    INT index = 0;
    index = CreateConstantBufferViews(device, incrementSize, index, m_cbProjection);
    index = CreateConstantBufferViews(device, incrementSize, index, m_cbView);
    index = CreateConstantBufferViews(device, incrementSize, index, m_cbModel);
}

} // namespace zxultra