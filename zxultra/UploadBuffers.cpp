#include "UploadBuffers.h"

namespace zxultra
{

ComPtr<ID3D12Resource> UploadBuffers::CreateDefaultBuffer(ID3D12Device *device,
                                                          ID3D12GraphicsCommandList *commandList,
                                                          const void *pData, size_t dataSize)
{
    ComPtr<ID3D12Resource> defaultBuffer;

    CD3DX12_HEAP_PROPERTIES heapPropertiesDefault{D3D12_HEAP_TYPE_DEFAULT};
    auto resourceDesc = CD3DX12_RESOURCE_DESC::Buffer(dataSize);
    HR(device->CreateCommittedResource(&heapPropertiesDefault, D3D12_HEAP_FLAG_NONE, &resourceDesc,
                                       D3D12_RESOURCE_STATE_COMMON, nullptr,
                                       IID_PPV_ARGS(defaultBuffer.GetAddressOf())));

    ComPtr<ID3D12Resource> uploadBuffer;

    CD3DX12_HEAP_PROPERTIES heapPropertiesUpload{D3D12_HEAP_TYPE_UPLOAD};
    HR(device->CreateCommittedResource(&heapPropertiesUpload, D3D12_HEAP_FLAG_NONE, &resourceDesc,
                                       D3D12_RESOURCE_STATE_GENERIC_READ, nullptr,
                                       IID_PPV_ARGS(uploadBuffer.GetAddressOf())));

    m_uploadBuffers.push_back(uploadBuffer);

    D3D12_SUBRESOURCE_DATA subresourceData{};
    subresourceData.pData = pData;
    subresourceData.RowPitch = dataSize;
    subresourceData.SlicePitch = dataSize;

    auto transition1 = CD3DX12_RESOURCE_BARRIER::Transition(
        defaultBuffer.Get(), D3D12_RESOURCE_STATE_COMMON, D3D12_RESOURCE_STATE_COPY_DEST);

    commandList->ResourceBarrier(1, &transition1);

    UpdateSubresources<1>(commandList, defaultBuffer.Get(), uploadBuffer.Get(), 0, 0, 1,
                          &subresourceData);

    auto transition2 = CD3DX12_RESOURCE_BARRIER::Transition(
        defaultBuffer.Get(), D3D12_RESOURCE_STATE_COPY_DEST, D3D12_RESOURCE_STATE_GENERIC_READ);

    commandList->ResourceBarrier(1, &transition2);

    return defaultBuffer;
}

} // namespace zxultra