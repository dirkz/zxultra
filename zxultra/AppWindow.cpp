#include "AppWindow.h"

#include <Dxgi.h>
#include <d3d12.h>

#include <format>
#include <string>

#include "Error.h"

using Microsoft::WRL::ComPtr;

namespace zxultra
{

void DXWindow::OnHwndCreated(HWND hwnd)
{
    LogAdapters();
}

void DXWindow::Resize(int width, int height)
{
}

void DXWindow::Update(double elapsedSeconds)
{
}

void DXWindow::Draw()
{
}

void DXWindow::LogAdapters()
{
    ComPtr<IDXGIFactory1> factory;
    ThrowIfFailed(CreateDXGIFactory1(IID_PPV_ARGS(factory.GetAddressOf())));

    ComPtr<IDXGIAdapter1> adapter;
    UINT i = 0;
    while (factory->EnumAdapters1(i, adapter.GetAddressOf()) != DXGI_ERROR_NOT_FOUND)
    {
        DXGI_ADAPTER_DESC1 description;
        adapter->GetDesc1(&description);

        std::wstring msg = std::format(L"Adapter: {}\n", description.Description);
        OutputDebugString(msg.c_str());

        ++i;
    }
}

} // namespace zxultra
