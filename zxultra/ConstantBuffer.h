#pragma once

#include "stdafx.h"

namespace zxultra
{

inline UINT RoundedBufferElementSize(UINT size)
{
    constexpr UINT ElementSize = 256;
    return size + (ElementSize - 1) & ~(ElementSize - 1);
}

template <class T> struct ConstantBuffer
{
    ConstantBuffer()
    {
    }

    ConstantBuffer(ID3D12Device *device, UINT numElements)
        : m_numElements{numElements}, m_elementSize{RoundedBufferElementSize(sizeof(T))}
    {
        UINT bufferSize = m_elementSize * numElements;

        auto heapProperties = CD3DX12_HEAP_PROPERTIES{D3D12_HEAP_TYPE_UPLOAD};
        auto resourceDescription = CD3DX12_RESOURCE_DESC::Buffer(bufferSize);

        HR(device->CreateCommittedResource(&heapProperties, D3D12_HEAP_FLAG_NONE,
                                           &resourceDescription, D3D12_RESOURCE_STATE_GENERIC_READ,
                                           nullptr, IID_PPV_ARGS(m_resource.GetAddressOf())));

        HR(m_resource->Map(0, nullptr, reinterpret_cast<void **>(&m_mappedBuffer)));
    }

    ConstantBuffer(const ConstantBuffer &) = delete;

    ConstantBuffer &operator=(const ConstantBuffer &) = delete;

    ConstantBuffer &operator=(ConstantBuffer &&other)
    {
        std::swap(m_numElements, other.m_numElements);
        std::swap(m_resource, other.m_resource);
        std::swap(m_mappedBuffer, other.m_mappedBuffer);
    }

    ~ConstantBuffer()
    {
        if (m_resource.Get() && m_mappedBuffer)
        {
            m_resource->Unmap(0, nullptr);
            m_resource = nullptr;
        }
    }

    inline UINT ElementSize() const
    {
        return m_elementSize;
    }

    inline UINT Size() const
    {
        return m_numElements;
    }

    inline ID3D12Resource *Resource() const
    {
        return m_resource.Get();
    }

    inline D3D12_GPU_VIRTUAL_ADDRESS GetGPUVirtualAddress(UINT n) const
    {
        return Resource()->GetGPUVirtualAddress() + n *m_elementSize;
    }

    inline BYTE *MappedBuffer() const
    {
        return m_mappedBuffer;
    }

    inline const T &operator[](size_t n) const
    {
        const T *buffer = reinterpret_cast<const T *>(m_mappedBuffer);
        return buffer[n];
    }

    inline T &operator[](size_t n)
    {
        T *buffer = reinterpret_cast<T *>(m_mappedBuffer);
        return buffer[n];
    }

    inline std::vector<D3D12_GPU_DESCRIPTOR_HANDLE> DescriptorHandles()
    {
        CD3DX12_GPU_DESCRIPTOR_HANDLE base{Resource()->GetGPUVirtualAddress()};

        std::vector<D3D12_GPU_DESCRIPTOR_HANDLE> handles{base};

        for (INT i = 1; i < Size(); ++i)
        {
            CD3DX12_GPU_DESCRIPTOR_HANDLE handle{base, i, ElementSize()};
            handles.push_back(handle);
        }

        return handles;
    }

  private:
    // Note: Move semantics!

    UINT m_elementSize = 0;
    UINT m_numElements = 0;
    ComPtr<ID3D12Resource> m_resource;
    BYTE *m_mappedBuffer = nullptr;
};

} // namespace zxultra
