#include <Windows.h>
#include <shlobj_core.h>
#include <string>
#include <gdiplus.h>
#include "SketchTimerApp.h"
#include "Wnd.h"
#include "winFuncitons.h"
#include "resource.h"
SketchTimerInitData initData;
SketchTimerApp sta;

BOOL CALLBACK initDataDlgProc(HWND, UINT, WPARAM, LPARAM);
LRESULT CALLBACK WndProc(HWND, UINT, WPARAM, LPARAM);

int WINAPI wWinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPWSTR lpCmdLine, int nCmdShow)
{
	MSG msg;

	DialogBox(hInstance, MAKEINTRESOURCEW(IDD_INIT_DATA), NULL, initDataDlgProc);

	Wnd mainWnd(L"Sketch Timer", hInstance, nCmdShow, WndProc, NULL);
	if (sta.IsOverlay())
		SetWindowPos(mainWnd.GetHWnd(), HWND_TOPMOST, 0, 0, GetSystemMetrics(SM_CXSCREEN) * 30 / 100, 100, NULL);

	sta.SetHWnd(mainWnd.GetHWnd());

	Gdiplus::GdiplusStartupInput si;
	ULONG_PTR token;
	Gdiplus::GdiplusStartup(&token, &si, 0);

	sta.Start();

	while (GetMessage(&msg, NULL, 0, 0) > 0)
	{
			TranslateMessage(&msg);
			DispatchMessageW(&msg);
	}

	return msg.wParam;
}


BOOL CALLBACK initDataDlgProc(HWND hDlg, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	static BROWSEINFOW  bi{};
	static wchar_t text[100]{};
	switch (uMsg)
	{
	case WM_INITDIALOG:
		bi.hwndOwner = hDlg;
		bi.lpszTitle = L"Chouse folder:";
		bi.pszDisplayName = text;
		return TRUE;

	case WM_COMMAND:
		switch (wParam)
		{
		case IDD_M_SELECT_FOLDER:
		{
			LPITEMIDLIST lpItem = SHBrowseForFolderW(&bi);
			if (lpItem)
			{
				SHGetPathFromIDListW(lpItem, text);
				initData._sourceDir = text;
			}
		}
			break;

		case IDOK:
			SendDlgItemMessageW(hDlg, IDD_M_NUM_PICTS, WM_GETTEXT, 100, (LPARAM)text);
			initData._numPict = _wtoi(text);
			SendDlgItemMessageW(hDlg, IDD_M_TIME_FOR_PIC, WM_GETTEXT, 100, (LPARAM)text);
			initData._timeForPict = _wtoi(text);
			sta.Initialize(initData);
			EndDialog(hDlg, TRUE);
			return TRUE;

		case IDCANCEL:
			PostQuitMessage(0);
			break;
		}
		break;
	}
	return FALSE;
}

LRESULT CALLBACK WndProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	static std::wstring time;
	static HDC hDC;
	PAINTSTRUCT ps;
	static RECT timeRect;
	RECT rc, pictRect;
	static LOGFONT lf;
	static HFONT hFontOld;
	static HBITMAP hBitmap;
	HFONT hFontNew;
	static wchar_t * pictName;
	
	//scale
	static double widthToHeightCoef;
	static int pictToWndAddW;
	static int pictToWndAddH;

	static double pictWidth = GetSystemMetrics(SM_CXSCREEN) * 30 / 100;
	static double pictHeight;

	static double windowWidth;
	static double windowHeight;
	
	//move
	static int x{};
	static int y{};
	
	//delete
	static int timelenght{};

	switch (uMsg)
	{
	case UM_SHOW_TIME:
		time = std::to_wstring((int)wParam);
		GetClientRect(hWnd, &rc);
		lf.lfHeight = 20;
		lf.lfWidth = 20;
		lstrcpyW((LPWSTR)&lf.lfFaceName, L"Lucida Console");
		timeRect.right = rc.right;
		timeRect.top = rc.top;
		if (timelenght < time.length())
			timelenght = time.length();
		timeRect.left = rc.right - (timelenght * lf.lfHeight);
		timeRect.bottom = lf.lfHeight;
		InvalidateRect(hWnd, &timeRect, TRUE);
		break;

	case UM_SHOW_PICT:
		pictName = (wchar_t*)wParam;
		InvalidateRect(hWnd, NULL, TRUE);
		break;
	
	case WM_SIZE:
		pictWidth = LOWORD(lParam);
		
		break;
	
	case WM_PAINT:
	{
		hDC = BeginPaint(hWnd, &ps);
		GetWindowRect(hWnd, &rc);
		GetClientRect(hWnd, &pictRect);
		pictToWndAddW = (rc.right - rc.left) - (pictRect.right - pictRect.left);
		pictToWndAddH = (rc.bottom - rc.top) - (pictRect.bottom - pictRect.top);
		Gdiplus::Image pict(pictName);
		Gdiplus::Graphics grap(hDC);
		widthToHeightCoef = (double)pict.GetHeight() / (double)pict.GetWidth();
		pictHeight = pictWidth * widthToHeightCoef;
		
		grap.DrawImage(&pict, 0, 0, pictWidth, pictHeight);
		windowWidth = pictWidth + pictToWndAddW;
		windowHeight = pictHeight + pictToWndAddH;
		SetWindowPos(hWnd, HWND_TOPMOST, 0, 0, windowWidth, windowHeight, SWP_NOMOVE);
		hFontNew = CreateFontIndirectW(&lf);
		hFontOld = (HFONT)SelectObject(hDC, hFontNew);
		timeRect.left = pictRect.right - (time.length() * lf.lfHeight);
		TextOutW(hDC, timeRect.left, 0, time.c_str(), time.size());
		DeleteObject(SelectObject(hDC, hFontOld));
		EndPaint(hWnd, &ps);
	}
		break;

	case WM_DESTROY:
		ExitProcess(0);
		break;

	default:
		return DefWindowProcW(hWnd, uMsg, wParam, lParam);
	}
	return 0;
}