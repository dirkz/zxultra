#pragma once

#include "stdafx.h"

namespace zxultra
{

struct DefaultBufferCreator
{
    DefaultBufferCreator(ID3D12Device *device);

    ComPtr<ID3D12Resource> CreateDefaultBuffer(ID3D12GraphicsCommandList *commandList,
                                               const void *pData, size_t dataSize);

    template <class T, std::size_t N = std::dynamic_extent>
    ComPtr<ID3D12Resource> CreateDefaultBuffer(ID3D12GraphicsCommandList *commandList,
                                               const std::span<T, N> &elements)
    {
        size_t dataSize = elements.size() * sizeof(T);
        const void *pData = elements.data();

        return CreateDefaultBuffer(commandList, pData, dataSize);
    };

  private:
    ComPtr<ID3D12Device> m_device;
    std::vector<ComPtr<ID3D12Resource>> m_uploadBuffers;
};

} // namespace zxultra
