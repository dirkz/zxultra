#include "AppWindow.h"

#include <Dxgi.h>
#include <d3d12.h>
#include <string>

#include "Error.h"

using Microsoft::WRL::ComPtr;

namespace zxultra
{

void DXWindow::OnHwndCreated(HWND hwnd)
{
    ComPtr<IDXGIFactory1> factory;
    ThrowIfFailed(CreateDXGIFactory1(IID_PPV_ARGS(factory.GetAddressOf())));
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

} // namespace zxultra
