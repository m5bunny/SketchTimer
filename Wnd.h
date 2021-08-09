#ifndef  WND_H_
#define WND_H_
#include <Windows.h>

class Wnd
{
	HWND hWnd;
	WNDCLASSEXW wc;
public:
	Wnd(LPCWSTR wndName, HINSTANCE hInstance, int cmdShow,
		LRESULT(WINAPI* pWndProc)(HWND, UINT, WPARAM, LPARAM),
		LPCWSTR menuName = NULL,
		int x = CW_USEDEFAULT, int y = 0,
		int cx = CW_USEDEFAULT, int cy = 0,
		DWORD windowStyle = WS_OVERLAPPEDWINDOW,
		UINT classStyle = CS_HREDRAW | CS_VREDRAW, 
		HWND hParent = NULL);
	const HWND GetHWnd() const { return hWnd; }
};
#endif // ! WND_H_

