#include <windows.h>

#include <string>

static LRESULT CALLBACK WindowProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
    switch (uMsg)
    {
    case WM_DESTROY:
        PostQuitMessage(0);
        return 0;

    case WM_PAINT: {
        PAINTSTRUCT ps;
        HDC hdc = BeginPaint(hwnd, &ps);

        // All painting occurs here, between BeginPaint and EndPaint.

        FillRect(hdc, &ps.rcPaint, (HBRUSH)(COLOR_WINDOW + 1));

        EndPaint(hwnd, &ps);
    }
        return 0;
    }

    return DefWindowProc(hwnd, uMsg, wParam, lParam);
}

static int CreateWindowAndRunMessageLoop(HINSTANCE hInstance, int nShowCmd)
{

    const std::wstring WindowClassName{L"zxultra_class"};

    WNDCLASS wc{};
    wc.lpfnWndProc = WindowProc;
    wc.hInstance = hInstance;
    wc.lpszClassName = WindowClassName.c_str();

    RegisterClass(&wc);

    // Determine window coordinates
    int width = GetSystemMetrics(SM_CXFULLSCREEN);
    int height = GetSystemMetrics(SM_CYFULLSCREEN);

    constexpr int desiredWidth = 1280;
    constexpr int desiredHeight = 800;

    int x = CW_USEDEFAULT;
    int y = CW_USEDEFAULT;

    if (width > desiredWidth && height > desiredHeight)
    {
        x = (width - desiredWidth) / 2;
        y = 0;
        width = desiredWidth;
        height = desiredHeight;
    }
    else
    {
        width = CW_USEDEFAULT;
        height = CW_USEDEFAULT;
    }

    HWND hwnd = CreateWindowEx(0, // optional windows styles
                               WindowClassName.c_str(),
                               L"zxultra",          // window title
                               WS_OVERLAPPEDWINDOW, // window style
                               x, y, width, height,
                               nullptr, // parent window
                               nullptr, // menu
                               hInstance,
                               nullptr // additional application state
    );

    if (hwnd == nullptr)
    {
        OutputDebugString(L"Could not create window, will abort");
        return 0;
    }

    ShowWindow(hwnd, nShowCmd);

    // Run the message loop.

    MSG msg = {};
    while (GetMessage(&msg, NULL, 0, 0) > 0)
    {
        TranslateMessage(&msg);
        DispatchMessage(&msg);
    }

    return static_cast<int>(msg.wParam);
}

int APIENTRY WinMain(_In_ HINSTANCE hInstance, _In_opt_ HINSTANCE hPrevInstance,
                     _In_ LPSTR lpCmdLine, _In_ int nShowCmd)
{
    OutputDebugString(L"Hello, world!\n");

    return CreateWindowAndRunMessageLoop(hInstance, nShowCmd);
}