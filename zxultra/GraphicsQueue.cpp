#include "GraphicsQueue.h"

namespace zxultra
{

static ComPtr<ID3D12CommandAllocator> CreateCommandAllocator(ID3D12Device *device)
{
    ComPtr<ID3D12CommandAllocator> commandAllocator;

    HR(device->CreateCommandAllocator(D3D12_COMMAND_LIST_TYPE_DIRECT,
                                      IID_PPV_ARGS(commandAllocator.GetAddressOf())));

    return commandAllocator;
}

static ComPtr<ID3D12GraphicsCommandList> CreateGraphicsCommandList(
    ID3D12Device *device, ID3D12CommandAllocator *commandAllocator)
{
    ComPtr<ID3D12GraphicsCommandList> commandList;

    HR(device->CreateCommandList(0, D3D12_COMMAND_LIST_TYPE_DIRECT, commandAllocator, nullptr,
                                 IID_PPV_ARGS(commandList.GetAddressOf())));

    return commandList;
}

GraphicsQueue::GraphicsQueue(ID3D12Device *device, ID3D12CommandQueue *commandQueue)
    : m_commandQueue{commandQueue}, m_commandAllocator{CreateCommandAllocator(device)},
      m_commandList{CreateGraphicsCommandList(device, m_commandAllocator.Get())}, m_fence{device}
{
}

void GraphicsQueue::Execute()
{
    HR(m_commandList->Close());
    ID3D12CommandList *commandLists[]{m_commandList.Get()};
    m_commandQueue->ExecuteCommandLists(_countof(commandLists), commandLists);
}

void GraphicsQueue::Flush()
{
    m_fence.SignalAndWait(CommandQueue());
}

void GraphicsQueue::Reset()
{
    HR(m_commandAllocator->Reset());
    HR(m_commandList->Reset(m_commandAllocator.Get(), nullptr));
}

ID3D12GraphicsCommandList *GraphicsQueue::operator->()
{
    return m_commandList.Get();
}

} // namespace zxultra