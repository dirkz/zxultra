#pragma once

#include "stdafx.h"

#include "ConstantBuffer.h"
#include "DescriptorHeap.h"

namespace zxultra
{

struct FrameData
{
    FrameData(ID3D12Device *device);

    ConstantBuffer<XMFLOAT4X4> &CbProjection()
    {
        return m_cbProjection;
    }

    ConstantBuffer<XMFLOAT4X4> &CbView()
    {
        return m_cbView;
    }

    ConstantBuffer<XMFLOAT4X4> &CbModel()
    {
        return m_cbModel;
    }

    template <class T>
    inline UINT CreateConstantBufferViews(ID3D12Device *device, UINT incrementSize,
                                          INT startingIndex,
                                          const ConstantBuffer<T> &constantBuffer)
    {
        INT index = startingIndex;

        CD3DX12_CPU_DESCRIPTOR_HANDLE base{m_descriptorHeap->GetCPUDescriptorHandleForHeapStart()};

        for (const D3D12_CONSTANT_BUFFER_VIEW_DESC &desc :
             constantBuffer.ConstantBufferViewDescriptions())
        {
            CD3DX12_CPU_DESCRIPTOR_HANDLE handle{base, index, incrementSize};
            device->CreateConstantBufferView(&desc, handle);

            index++;
        }

        return index;
    }

  private:
    ConstantBuffer<XMFLOAT4X4> m_cbProjection;
    ConstantBuffer<XMFLOAT4X4> m_cbView;
    ConstantBuffer<XMFLOAT4X4> m_cbModel;

    DescriptorHeap m_descriptorHeap;
};

} // namespace zxultra
