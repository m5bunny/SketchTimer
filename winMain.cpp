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
	Gdiplus::GdiplusStartupInput si;
	ULONG_PTR token;

	DialogBox(hInstance, MAKEINTRESOURCEW(IDD_INIT_DATA), NULL, initDataDlgProc);

	Wnd mainWnd(L"Sketch Timer", hInstance, nCmdShow, WndProc, NULL, CW_USEDEFAULT, 0, 
		GetSystemMetrics(SM_CXSCREEN) * 30 / 100);

	Gdiplus::GdiplusStartup(&token, &si, 0);
	
	sta.SetHWnd(mainWnd.GetHWnd());
	sta.Start();

	while (GetMessage(&msg, NULL, 0, 0) > 0)
	{
		TranslateMessage(&msg);
		DispatchMessageW(&msg);
	}

	Gdiplus::GdiplusShutdown(token);
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
	RECT rc, pictRect, timeRect;
	static LOGFONT lf;
	static HFONT hFontOld;
	static HBITMAP hBitmap;
	static HFONT hFont;
	static wchar_t* pictName;

	static double widthToHeightCoef;
	static int pictToWndAddW;
	static int pictToWndAddH;
	static double pictWidth;
	static double pictHeight;
	static double windowWidth;
	static double windowHeight;

	static Gdiplus::Image * pict;

	static HDC hMemDC;
	static HBITMAP hMemBmp;
	static HWND overlay = NULL;

	switch (uMsg)
	{
	case UM_SHOW_TIME:
		time = std::to_wstring((int)wParam);
		lstrcpyW((LPWSTR)&lf.lfFaceName, L"Lucida Console");
		lf.lfHeight = 20;
		lf.lfWidth = 20;
		hFont = CreateFontIndirectW(&lf);
		InvalidateRect(hWnd, NULL, TRUE);
		break;

	case UM_SHOW_PICT:
		pictName = (wchar_t*)wParam;
		pict = Gdiplus::Image::FromFile(pictName);
		widthToHeightCoef = (double)pict->GetHeight() / (double)pict->GetWidth();
		pictHeight = pictWidth * widthToHeightCoef;
		windowWidth = pictWidth + pictToWndAddW;
		windowHeight = pictHeight + pictToWndAddH;

		if (sta.IsOverlay())
			overlay = HWND_TOPMOST;

		SetWindowPos(hWnd, overlay, 0, 0, windowWidth, windowHeight, SWP_NOMOVE | SWP_NOREDRAW | SWP_NOSENDCHANGING);

		SendMessageW(hWnd, UM_UPDATE_PICT, 0, 0);
	break;

	case UM_UPDATE_PICT:
	{
		GetWindowRect(hWnd, &rc);
		GetClientRect(hWnd, &pictRect);
		pictToWndAddW = (rc.right - rc.left) - (pictRect.right - pictRect.left);
		pictToWndAddH = (rc.bottom - rc.top) - (pictRect.bottom - pictRect.top);
		hDC = GetDC(hWnd);
		hMemDC = CreateCompatibleDC(hDC);
		hMemBmp = CreateCompatibleBitmap(hDC, pictWidth, pictHeight);
		SelectObject(hMemDC, hMemBmp);
		Gdiplus::Graphics grap(hMemDC);
		grap.DrawImage(pict, 0, 0, pictWidth, pictHeight);
	}
		break;

	case WM_SIZE:
		pictWidth = LOWORD(lParam);
		pictHeight = pictWidth * widthToHeightCoef;
		windowWidth = pictWidth + pictToWndAddW;
		windowHeight = pictHeight + pictToWndAddH;
		SendMessageW(hWnd, UM_UPDATE_PICT, 0, 0);
		break;

	case WM_GETMINMAXINFO:
	{
		LPMINMAXINFO lpMMI = (LPMINMAXINFO)lParam;
		lpMMI->ptMinTrackSize.y = lpMMI->ptMaxTrackSize.y = windowHeight;
	}

	case WM_PAINT:
		hDC = BeginPaint(hWnd, &ps);
		GetClientRect(hWnd, &pictRect);
		BitBlt(hDC, 0, 0, pictRect.right, pictRect.bottom, hMemDC, 0, 0, SRCCOPY);
		hFontOld = (HFONT)SelectObject(hDC, hFont);
		timeRect.left = pictRect.right - (time.length() * lf.lfHeight);
		TextOutW(hDC, timeRect.left, 0, time.c_str(), time.size());
		SelectObject(hDC, hFontOld);
		EndPaint(hWnd, &ps);
	break;

	case WM_ERASEBKGND:
		return 0;

	case WM_KEYDOWN:
		if (wParam == VK_SPACE)
			sta.ChangePaused();
		if (wParam == VK_RIGHT)
			sta.SkipPict();
		break;

	case WM_DESTROY:
		DeleteDC(hMemDC);
		DeleteObject(hMemBmp);
		DeleteObject(hFont);
		ExitProcess(0);
		break;

	default:
		return DefWindowProcW(hWnd, uMsg, wParam, lParam);
	}
	return 0;
}