#include "Wnd.h"

Wnd::Wnd(LPCWSTR wndName, HINSTANCE hInstance, int cmdShow,
	LRESULT(WINAPI* pWndProc)(HWND, UINT, WPARAM, LPARAM),
	LPCWSTR menuName,
	DWORD windowStyle,
	UINT classStyle,
	int x, int y,
	int cx, int cy,
	HWND hParent)
{
	const wchar_t className[] = L"Wnd";

	wc.cbSize = sizeof(wc);
	wc.style = classStyle;
	wc.lpfnWndProc = pWndProc;
	wc.cbClsExtra = 0;
	wc.cbWndExtra = 0;
	wc.hInstance = hInstance;
	wc.hIcon = LoadIconW(NULL, IDI_APPLICATION);
	wc.hCursor = LoadCursorW(NULL, IDC_ARROW);
	wc.hbrBackground = (HBRUSH)GetStockObject(WHITE_BRUSH);
	wc.lpszMenuName = menuName;
	wc.lpszClassName = className;
	wc.hIconSm = LoadIconW(NULL, IDI_APPLICATION);

	if (!RegisterClassExW(&wc))
	{
		MessageBoxW(NULL, L"Error of registering window class", L"Error", MB_OK | MB_ICONWARNING);
		return;
	}

	hWnd = CreateWindowW(wc.lpszClassName, wndName, windowStyle, x, y, cx, cy, hParent, 
		(HMENU)NULL, hInstance, NULL);

	if (!hWnd)
	{
		MessageBoxW(NULL, L"Error of creating the window", L"Error", MB_OK | MB_ICONWARNING);
		return;
	}

	ShowWindow(hWnd, cmdShow);
}