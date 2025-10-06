#include <windows.h>

#include <string>

#include "BaseWindow.h"

using namespace zfxultra;

int APIENTRY WinMain(_In_ HINSTANCE hInstance, _In_opt_ HINSTANCE hPrevInstance,
                     _In_ LPSTR lpCmdLine, _In_ int nShowCmd)
{
    OutputDebugString(L"Hello, world!\n");

    BaseWindow<int> window{hInstance, L"ZFX Ultra", 1280, 800};
    window.ShowWindow(nShowCmd);

    return window.RunMessageLoop();
}