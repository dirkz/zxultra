#include "DXWindow.h"

#include <string>

namespace zxultra
{

void DXWindow::OnHwndCreated(HWND hwnd)
{
    OutputDebugString(L"CreatedHwnd()");
}

void DXWindow::Update(double elapsedSeconds)
{
    OutputDebugString(L"Update()\n");
}

void DXWindow::Draw()
{
    OutputDebugString(L"Draw()\n");
}

} // namespace zxultra
