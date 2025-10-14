#pragma once

#include "stdafx.h"

namespace zxultra
{

struct UploadBuffers
{
    ComPtr<ID3D12Resource> CreateDefaultBuffer(ID3D12Device *device,
                                               ID3D12GraphicsCommandList *commandList,
                                               const void *pData, size_t dataSize);

    template <class T>
    ComPtr<ID3D12Resource> CreateDefaultBuffer(ID3D12Device *device,
                                               ID3D12GraphicsCommandList *commandList,
                                               std::span<T> elements)
    {
        size_t dataSize = elements.size() * sizeof(T);
        const void *pData = elements.data();

        return CreateDefaultBuffer(device, commandList, pData, dataSize);
    };

  private:
    std::vector<ComPtr<ID3D12Resource>> m_uploadBuffers;
};

} // namespace zxultra
