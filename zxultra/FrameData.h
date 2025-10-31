#pragma once

#include "stdafx.h"

#include "ConstantBuffer.h"
#include "Constants.h"
#include "CreateFunctions.h"
#include "DescriptorHeap.h"
#include "Fence.h"

namespace zxultra
{

/// <summary>
/// Per-frame data
/// </summary>
/// <typeparam name="P">The per-pass object type</typeparam>
/// <typeparam name="O">The per-object object type</typeparam>
/// <typeparam name="I">The number of per-object objects</typeparam>
template <class P, class O, size_t I> struct FrameData
{
    /// <summary>
    /// The number of entries in the root signature.
    /// There are 2 distinct object types (per-pass and per-object),
    /// so this is 2.
    /// </summary>
    /// <returns></returns>
    static const UINT NumRootParameters = 2;

    static ComPtr<ID3D12RootSignature> CreateRootSignature(ID3D12Device *device)
    {
        CD3DX12_ROOT_PARAMETER rootParameters[NumRootParameters]{};

        CD3DX12_DESCRIPTOR_RANGE descriptorRange0{}, descriptorRange1{};
        constexpr UINT baseShaderRegister = 0;
        constexpr UINT registerSpace = 0;
        constexpr UINT offsetInDescriptorsFromTableStart = 0;

        descriptorRange0.Init(D3D12_DESCRIPTOR_RANGE_TYPE_CBV, 1, baseShaderRegister, registerSpace,
                              offsetInDescriptorsFromTableStart);
        rootParameters[0].InitAsDescriptorTable(1, &descriptorRange0);

        descriptorRange1.Init(D3D12_DESCRIPTOR_RANGE_TYPE_CBV, 1, baseShaderRegister + 1,
                              registerSpace, offsetInDescriptorsFromTableStart);
        rootParameters[1].InitAsDescriptorTable(1, &descriptorRange1);

        constexpr UINT numStaticSamples = 0;
        constexpr D3D12_STATIC_SAMPLER_DESC *samplerDescription = nullptr;
        CD3DX12_ROOT_SIGNATURE_DESC rootSignatureDescription{
            _countof(rootParameters), rootParameters, numStaticSamples, samplerDescription,
            D3D12_ROOT_SIGNATURE_FLAG_ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT};

        ComPtr<ID3DBlob> serializedRootSignature = nullptr;
        ComPtr<ID3DBlob> errorBlob = nullptr;
        HRESULT hr = D3D12SerializeRootSignature(
            &rootSignatureDescription, D3D_ROOT_SIGNATURE_VERSION_1,
            serializedRootSignature.GetAddressOf(), errorBlob.GetAddressOf());

        if (!SUCCEEDED(hr) && errorBlob.Get() != nullptr)
        {
            const char *msg = reinterpret_cast<char *>(errorBlob->GetBufferPointer());
            OutputDebugString(L"D3D12SerializeRootSignature failed with the following error:\n\n");
            OutputDebugStringA(msg);
            OutputDebugString(L"\n");
        }

        HR(hr);

        ComPtr<ID3D12RootSignature> rootSignature;

        constexpr UINT nodeMask = 0;
        HR(device->CreateRootSignature(nodeMask, serializedRootSignature->GetBufferPointer(),
                                       serializedRootSignature->GetBufferSize(),
                                       IID_PPV_ARGS(rootSignature.GetAddressOf())));

        return rootSignature;
    }

    FrameData(ID3D12Device *device)
        : m_commandAllocator{CreateCommandAllocator(device)}, m_fence{device},
          m_cbPerPass{device, 1}, m_cbPerObject{device, I},
          m_descriptorHeap{device, I + 1, D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV,
                           D3D12_DESCRIPTOR_HEAP_FLAG_SHADER_VISIBLE}
    {
        // All per-object entries first
        INT index = 0;
        m_perObjectDescriptorIndex = index;
        index = m_descriptorHeap.CreateConstantBufferViews(index, m_cbPerObject);

        // Then the per-pass data
        m_perPassDescriptorIndex = index;
        index = m_descriptorHeap.CreateConstantBufferViews(index, m_cbPerPass);
    }

    FrameData(const FrameData &) = delete;

    FrameData &operator=(const FrameData &) = delete;

    P &PerPass()
    {
        return m_cbPerPass[0];
    }

    O &PerObject(const UINT index)
    {
        return m_cbPerObject[index];
    }

    std::array<ID3D12DescriptorHeap *, 1> DescriptorHeaps()
    {
        return {m_descriptorHeap.Get()};
    }

    inline ID3D12CommandAllocator *CommandAllocator() const
    {
        return m_commandAllocator.Get();
    }

    void FlushFence(ID3D12CommandQueue *commandQueue)
    {
        m_fence.Flush(commandQueue);
    }

    CD3DX12_GPU_DESCRIPTOR_HANDLE PerPassGPUDescriptorHandle() const
    {
        return m_descriptorHeap.GetGPUDescriptorHandle(m_perPassDescriptorIndex);
    }

    CD3DX12_GPU_DESCRIPTOR_HANDLE PerObjectGPUDescriptorHandle(UINT index) const
    {
        return m_descriptorHeap.GetGPUDescriptorHandle(m_perObjectDescriptorIndex + index);
    }

  private:
    ComPtr<ID3D12CommandAllocator> m_commandAllocator;

    Fence m_fence;

    ConstantBuffer<P> m_cbPerPass;
    ConstantBuffer<O> m_cbPerObject;

    DescriptorHeap m_descriptorHeap;

    UINT m_perObjectDescriptorIndex = 0;
    UINT m_perPassDescriptorIndex = 0;
};

} // namespace zxultra
