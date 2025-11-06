#include <windows.h>

#include <string>

#include "AppWindow.h"
#include "BaseWindow.h"

using namespace zxultra;

int APIENTRY WinMain(_In_ HINSTANCE hInstance, _In_opt_ HINSTANCE hPrevInstance,
                     _In_ LPSTR lpCmdLine, _In_ int nShowCmd)
{
    int result = 0;

    {
        BaseWindow<AppWindow> baseWindow{hInstance, L"ZFX Ultra", 1280, 720};
        baseWindow.ShowWindow(nShowCmd);

        result = baseWindow.RunMessageLoop();
    }

    return result;
}