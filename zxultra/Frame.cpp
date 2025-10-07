#include "Frame.h"

namespace zxultra
{

Frame::Frame(ID3D12Device *device) : m_fence{device}
{
    D3D12_COMMAND_QUEUE_DESC queueDesc{};
    queueDesc.Type = D3D12_COMMAND_LIST_TYPE_DIRECT;
    queueDesc.Flags = D3D12_COMMAND_QUEUE_FLAG_NONE;
    HR(device->CreateCommandQueue(&queueDesc, IID_PPV_ARGS(m_commandQueue.GetAddressOf())));

    HR(device->CreateCommandAllocator(D3D12_COMMAND_LIST_TYPE_DIRECT,
                                      IID_PPV_ARGS(m_commandAllocator.GetAddressOf())));

    HR(device->CreateCommandList(0, D3D12_COMMAND_LIST_TYPE_DIRECT, m_commandAllocator.Get(),
                                 nullptr, IID_PPV_ARGS(m_commandList.GetAddressOf())));

    m_commandList->Close();
}

} // namespace zxultra