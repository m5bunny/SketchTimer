#ifndef KWNDEX_H_
#define KWNDEX_H_
#include <windows.h>
#include <commctrl.h>
#include <stdio.h>

class KWnd
{
public:
	KWnd(LPCTSTR windowName, HINSTANCE hIstance, int cmdShow,
		LRESULT(WINAPI * pWndProc)(HWND, UINT, WPARAM, LPARAM),
		LPCTSTR menuName = NULL,
		int x = CW_USEDEFAULT, int y = 0,
		int width = CW_USEDEFAULT, int height = 0,
		UINT classStyle = CS_HREDRAW | CS_VREDRAW,
		DWORD windowStyle = WS_OVERLAPPEDWINDOW,
		HWND hParent = NULL);
	HWND GetHWnd() { return hWnd; }
protected:
	HWND hWnd;
	WNDCLASSEX wc;
};

//Utilities
//Shift the main window with changing the size
void ShiftWindow(HWND hWnd, int dX = 0, int dY = 0, int dW = 0, int dH = 0);
//Shift the child window with changing the size
void ShiftWindow(HWND hChild, HWND hParent, int dX = 0, int dY = 0, int dW = 0, int dH = 0);
//Shift the control window with changing the size
void ShiftWindow(int ctrlID, HWND hParent, int dX = 0, int dY = 0, int dW = 0, int dH = 0);

//Add tip window
void AddTooltip(HWND hWndOwner, LPWSTR lpMsg);

//void TRACE(LPCTSTR lpszFormat, ...);

#endif // !KWND_H_