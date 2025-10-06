#include <windows.h>

#include <string>

#include "BaseWindow.h"

#include "DXWindow.h"

using namespace zxultra;

int APIENTRY WinMain(_In_ HINSTANCE hInstance, _In_opt_ HINSTANCE hPrevInstance,
                     _In_ LPSTR lpCmdLine, _In_ int nShowCmd)
{
    DXWindow dxWindow{};

    BaseWindow baseWindow{hInstance, L"ZFX Ultra", 1280, 800, &dxWindow};
    baseWindow.ShowWindow(nShowCmd);

    return baseWindow.RunMessageLoop();
}