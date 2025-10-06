#include <windows.h>

#include <format>
#include <stdexcept>
#include <string>

#include "DXWindow.h"
#include "Timer.h"

namespace zxultra
{

template <class T>
static LRESULT CALLBACK WindowProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam);

template <class T> struct BaseWindow
{
    BaseWindow(HINSTANCE hInstance, const std::wstring &windowTitle, int desiredWidth,
               int desiredHeight, T *appWindow)
        : m_appWindow{appWindow}
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

        appWindow->OnHwndCreated(hwnd);
    }

    inline void ShowWindow(int nShowCmd)
    {
        ::ShowWindow(m_hwnd, nShowCmd);
    }

    inline int RunMessageLoop()
    {
        MSG msg = {};
        while (GetMessage(&msg, NULL, 0, 0) > 0)
        {
            TranslateMessage(&msg);
            DispatchMessage(&msg);
        }

        return static_cast<int>(msg.wParam);
    }

    inline void StartTimer()
    {
        m_timer.Start();
    }

    inline void OnHwndCreated(HWND hwnd)
    {
        m_appWindow->OnHwndCreated(hwnd);
    }

    inline void Update()
    {
        double elapsedSeconds = m_timer.ElapsedSeconds();
        m_appWindow->Update(elapsedSeconds);
    }

    inline void Draw()
    {
        m_appWindow->Draw();
    }

  private:
    HWND m_hwnd;
    T *m_appWindow;
    Timer m_timer;
};

template <class T>
static LRESULT CALLBACK WindowProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
    LONG_PTR lptr = GetWindowLongPtr(hwnd, GWLP_USERDATA);
    if (lptr)
    {
        BaseWindow<T> *baseWindow = reinterpret_cast<BaseWindow<T> *>(lptr);

        switch (uMsg)
        {
        case WM_CREATE:
            baseWindow->StartTimer();
            return 0;

        case WM_DESTROY:
            PostQuitMessage(0);
            return 0;

        case WM_PAINT:
            baseWindow->Update();
            baseWindow->Draw();
            return 0;
        }
    }
    else
    {
        switch (uMsg)
        {
        case WM_NCCREATE: {
            CREATESTRUCT *pCreateStruct = reinterpret_cast<CREATESTRUCT *>(lParam);
            BaseWindow<T> *pBaseWindow =
                reinterpret_cast<BaseWindow<T> *>(pCreateStruct->lpCreateParams);
            SetWindowLongPtr(hwnd, GWLP_USERDATA, reinterpret_cast<LONG_PTR>(pBaseWindow));
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

} // namespace zxultra