#pragma once

#include "stdafx.h"

namespace zxultra
{

struct DescriptorHandleSizes
{
    DescriptorHandleSizes(ID3D12Device *device)
    {
        m_rtvDescriptorHandleIncrementSize =
            device->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_RTV);
        m_dsvDescriptorHandleIncrementSize =
            device->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_DSV);
        m_cbvSrvUavDescriptorHandleIncrementSize =
            device->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV);
    }

    UINT RtvDescriptorHandleIncrementSize() const
    {
        return m_rtvDescriptorHandleIncrementSize;
    }

    UINT DsvDescriptorHandleIncrementSize() const
    {
        return m_dsvDescriptorHandleIncrementSize;
    }

    UINT CbvDescriptorHandleIncrementSize() const
    {
        return m_cbvSrvUavDescriptorHandleIncrementSize;
    }

    UINT SrvDescriptorHandleIncrementSize() const
    {
        return m_cbvSrvUavDescriptorHandleIncrementSize;
    }

    UINT UaDescriptorHandleIncrementSize() const
    {
        return m_cbvSrvUavDescriptorHandleIncrementSize;
    }

  private:
    UINT m_rtvDescriptorHandleIncrementSize;
    UINT m_dsvDescriptorHandleIncrementSize;
    UINT m_cbvSrvUavDescriptorHandleIncrementSize;
};

} // namespace zxultra
