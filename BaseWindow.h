#include <windows.h>

#include <format>
#include <stdexcept>
#include <string>

namespace zfxultra
{

template <class T>
static LRESULT CALLBACK WindowProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
    LONG_PTR lptr = GetWindowLongPtr(hwnd, GWLP_USERDATA);
    if (lptr)
    {
        T *appWindow = reinterpret_cast<T *>(lptr);

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
    }
    else
    {
        switch (uMsg)
        {
        case WM_NCCREATE: {
            CREATESTRUCT *pCreateStruct = reinterpret_cast<CREATESTRUCT *>(lParam);
            T *pApp = reinterpret_cast<T *>(pCreateStruct->lpCreateParams);
            SetWindowLongPtr(hwnd, GWLP_USERDATA, reinterpret_cast<LONG_PTR>(pApp));
        }
            return TRUE;

        default:
            std::wstring msgString = std::format(L"Received objectless message {}\n", uMsg);
            OutputDebugString(msgString.c_str());
            break;
        }
    }

    return DefWindowProc(hwnd, uMsg, wParam, lParam);
}

template <class T> struct BaseWindow
{
    BaseWindow(HINSTANCE hInstance, const std::wstring &windowTitle, int desiredWidth,
               int desiredHeight, T &window)
    {
        const std::wstring WindowClassName{L"zxultra_class"};

        WNDCLASS wc{};
        wc.lpfnWndProc = WindowProc<T>;
        wc.hInstance = hInstance;
        wc.lpszClassName = WindowClassName.c_str();

        RegisterClass(&wc);

        // Determine window coordinates
        int width = GetSystemMetrics(SM_CXFULLSCREEN);
        int height = GetSystemMetrics(SM_CYFULLSCREEN);

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
                                   windowTitle.c_str(), // window title
                                   WS_OVERLAPPEDWINDOW, // window style
                                   x, y, width, height,
                                   nullptr, // parent window
                                   nullptr, // menu
                                   hInstance,
                                   this // additional application state
        );

        if (hwnd == nullptr)
        {
            throw std::runtime_error{"Could not create window"};
        }

        m_hwnd = hwnd;

        window.OnHwndCreated(hwnd);
    }

    void ShowWindow(int nShowCmd)
    {
        ::ShowWindow(m_hwnd, nShowCmd);
    }

    int RunMessageLoop()
    {
        MSG msg = {};
        while (GetMessage(&msg, NULL, 0, 0) > 0)
        {
            TranslateMessage(&msg);
            DispatchMessage(&msg);
        }

        return static_cast<int>(msg.wParam);
    }

  private:
    HWND m_hwnd;
};

} // namespace zfxultra