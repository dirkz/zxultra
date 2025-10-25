#include "CommandList.h"

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

CommandList::CommandList(ID3D12Device *device)
    : m_commandAllocator{CreateCommandAllocator(device)},
      m_commandList{CreateGraphicsCommandList(device, m_commandAllocator.Get())}
{
}

void CommandList::Execute(ID3D12CommandQueue *commandQueue)
{
    HR(m_commandList->Close());
    ID3D12CommandList *commandLists[]{m_commandList.Get()};
    commandQueue->ExecuteCommandLists(_countof(commandLists), commandLists);
}

void CommandList::Reset(ID3D12PipelineState *pipelineState)
{
    HR(m_commandAllocator->Reset());
    HR(m_commandList->Reset(m_commandAllocator.Get(), pipelineState));
}

} // namespace zxultra