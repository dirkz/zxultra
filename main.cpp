#include <windows.h>

#include <string>

#include "AppWindow.h"
#include "BaseWindow.h"

using namespace zxultra;

int APIENTRY WinMain(_In_ HINSTANCE hInstance, _In_opt_ HINSTANCE hPrevInstance,
                     _In_ LPSTR lpCmdLine, _In_ int nShowCmd)
{
    BaseWindow<DXWindow> baseWindow{hInstance, L"ZFX Ultra", 1280, 800};
    baseWindow.ShowWindow(nShowCmd);

    return baseWindow.RunMessageLoop();
}