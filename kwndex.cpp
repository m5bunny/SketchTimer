#include "kwndex.h"

KWnd::KWnd(LPCTSTR windowName, HINSTANCE hIstanse, int cmdShow,
			LRESULT (WINAPI *pWndProc)(HWND, UINT, WPARAM, LPARAM),
			LPCTSTR menuName, int x, int y, int width, int height,
			UINT classStyle, DWORD windowStyle, HWND hParent)
{
	const wchar_t szClassName[] = L"KWndClass";

	wc.cbSize = sizeof(wc);
	wc.style = classStyle;
	wc.lpfnWndProc = pWndProc;
	wc.cbClsExtra = 0;
	wc.cbWndExtra = 0;
	wc.hInstance = hIstanse;
	wc.hIcon = LoadIcon(NULL, IDI_APPLICATION);
	wc.hCursor = LoadCursor(NULL, IDC_ARROW);
	wc.hbrBackground = (HBRUSH)GetStockObject(WHITE_BRUSH);
	wc.lpszMenuName = menuName;
	wc.lpszClassName = szClassName;
	wc.hIconSm = LoadIcon(NULL, IDI_APPLICATION);

	if (!RegisterClassEx(&wc))
	{
		wchar_t msg[100] = L"Cannot register class: ";
		MessageBox(NULL, wcscat(msg, szClassName), L"Error", MB_OK);
		return;
	}

	hWnd = CreateWindow(szClassName, windowName, windowStyle, 
						x, y, width, height, hParent, (HMENU)NULL, hIstanse, NULL);
	if (!hWnd)
	{
		wchar_t msg[100] = L"Cannot create a window: ";
		MessageBox(NULL, wcscat(msg, szClassName), L"Error", MB_OK);
		return;
	}

	ShowWindow(hWnd, cmdShow);

	INITCOMMONCONTROLSEX icc;
	icc.dwSize = sizeof(INITCOMMONCONTROLSEX);
	icc.dwICC = ICC_WIN95_CLASSES;
	InitCommonControlsEx(&icc);
}

//Utilities
void ShiftWindow(HWND hWnd, int dX, int dY, int dW, int dH)
{
	RECT rect;

	GetWindowRect(hWnd, &rect);
	int x0 = rect.left + dX;
	int y0 = rect.top + dY;
	int width = rect.right - rect.left + dW;
	int height = rect.bottom - rect.top + dH;
	MoveWindow(hWnd, x0, y0, width, height, TRUE);
}

void ShiftWindow(HWND hChild, HWND hParent, int dX, int dY, int dW, int dH)
{
	RECT rect;
	POINT p0;

	GetWindowRect(hChild, &rect);
	int width = rect.right - rect.left + dW;
	int height = rect.bottom - rect.top + dH;

	p0.x = rect.left + dX;
	p0.y = rect.top + dY;
	ScreenToClient(hParent, &p0);

	MoveWindow(hChild, p0.x, p0.y, width, height, TRUE);
}

void ShiftWindow(int ctrlID, HWND hParent, int dX, int dY, int dW, int dH)
{
	RECT rect;
	HWND hCtrl = GetDlgItem(hParent, ctrlID);
	POINT p0;

	GetWindowRect(hCtrl, &rect);
	int width = rect.right - rect.left + dW;
	int height = rect.bottom - rect.top + dH;

	p0.x = rect.left + dX;
	p0.y = rect.top + dY;
	ScreenToClient(hParent, &p0);

	ShowWindow(hCtrl, SW_HIDE);
	MoveWindow(hCtrl, p0.x, p0.y, width, height, TRUE);
	ShowWindow(hCtrl, SW_SHOW);
}

void AddTooltip(HWND hWndOwner, LPWSTR lpMsg)
{
	HWND hWndTip;
	TOOLINFO ti = { 0 };

	HINSTANCE hInst = (HINSTANCE)GetWindowLongPtr(hWndOwner, GWLP_HINSTANCE);
	
	hWndTip = CreateWindowEx(WS_EX_TOPMOST, TOOLTIPS_CLASS, NULL,
							 WS_POPUP | TTS_NOPREFIX | TTS_ALWAYSTIP,
							 CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT,
						     hWndOwner, NULL, hInst, NULL);

	SetWindowPos(hWndTip, HWND_TOPMOST, 0, 0, 0, 0,
		SWP_NOMOVE | SWP_NOSIZE | SWP_NOACTIVATE);

	ti.cbSize = sizeof(TOOLINFO);
	ti.uFlags = TTF_SUBCLASS;
	ti.hwnd = hWndOwner;
	ti.hinst = hInst;
	ti.lpszText = lpMsg;

	GetClientRect(hWndOwner, &ti.rect);

	SendMessage(hWndTip, TTM_ADDTOOL, 0, (LPARAM)(LPTOOLINFO)&ti);
}

#define N 512
/*
void TRACE(LPCTSTR lpszFormat, ...)
{
	va_list args;
	va_start(args, lpszFormat);
	int nBuf;
	wchar_t szBuffer[N];

	nBuf  = _vsnwprintf(szBuffer, N, lpszFormat, args);

	if (nBuf < 0)
	{
		MessageBox(NULL, L"To long string for TRACE", NULL, MB_OK);
		szBuffer[N - 2] = '\n';
		szBuffer[N - 1] = 0;
	}

	OutputDebugString(szBuffer);
	va_end(args);
}*/