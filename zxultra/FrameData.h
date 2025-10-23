#pragma once

#include "stdafx.h"

#include "ConstantBuffer.h"

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

  private:
    ConstantBuffer<XMFLOAT4X4> m_cbProjection;
    ConstantBuffer<XMFLOAT4X4> m_cbView;
    ConstantBuffer<XMFLOAT4X4> m_cbModel;

    ComPtr<ID3D12DescriptorHeap> m_descriptorHeap;
};

} // namespace zxultra
