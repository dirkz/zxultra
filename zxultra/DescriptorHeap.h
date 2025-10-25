#pragma once

#include "stdafx.h"

#include "ConstantBuffer.h"

namespace zxultra
{

struct DescriptorHeap
{
    DescriptorHeap(ID3D12Device *device, UINT numDescriptors, D3D12_DESCRIPTOR_HEAP_TYPE type,
                   D3D12_DESCRIPTOR_HEAP_FLAGS flags);

    template <class T>
    inline UINT CreateConstantBufferViews(INT startingIndex,
                                          const ConstantBuffer<T> &constantBuffer)
    {
        INT index = startingIndex;

        CD3DX12_CPU_DESCRIPTOR_HANDLE base{m_descriptorHeap->GetCPUDescriptorHandleForHeapStart()};

        for (const D3D12_CONSTANT_BUFFER_VIEW_DESC &desc :
             constantBuffer.ConstantBufferViewDescriptions())
        {
            CD3DX12_CPU_DESCRIPTOR_HANDLE handle{base, index, m_descriptorHandleIncrementSize};
            m_device->CreateConstantBufferView(&desc, handle);

            index++;
        }

        return index;
    }

    ID3D12DescriptorHeap *Get()
    {
        return m_descriptorHeap.Get();
    }

    ID3D12DescriptorHeap *operator*()
    {
        return m_descriptorHeap.Get();
    }

    ID3D12DescriptorHeap *operator->()
    {
        return m_descriptorHeap.Get();
    }

  private:
    ComPtr<ID3D12Device> m_device;
    ComPtr<ID3D12DescriptorHeap> m_descriptorHeap;
    UINT m_descriptorHandleIncrementSize;
};

} // namespace zxultra
