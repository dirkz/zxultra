#include "Fence.h"

namespace zxultra
{

Fence::Fence(ID3D12Device *device)
{
    HR(device->CreateFence(m_value, D3D12_FENCE_FLAG_NONE, IID_PPV_ARGS(m_fence.GetAddressOf())));
}

void Fence::SignalAndWait(ID3D12CommandQueue *commandQueue)
{
    m_value++;

    HR(commandQueue->Signal(m_fence.Get(), m_value));

    if (m_fence->GetCompletedValue() < m_value)
    {
        HANDLE eventHandle = CreateEventEx(nullptr, nullptr, 0, EVENT_ALL_ACCESS);
        if (eventHandle)
        {
            HR(m_fence->SetEventOnCompletion(m_value, eventHandle));
            WaitForSingleObject(eventHandle, INFINITE);
            CloseHandle(eventHandle);
        }
        else
        {
            CheckLastError();
        }
    }
}

} // namespace zxultra