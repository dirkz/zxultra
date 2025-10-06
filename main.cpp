#include <windows.h>

int APIENTRY WinMain(_In_ HINSTANCE hInstance, _In_opt_ HINSTANCE hPrevInstance, _In_ LPSTR lpCmdLine, _In_ int nShowCmd)
{
	OutputDebugString(L"Hello, world!\n");

	// TODO: Return wParam of WM_QUIT message.
	// https://learn.microsoft.com/en-us/windows/win32/api/winbase/nf-winbase-winmain
	return 0;
}