#include <Windows.h>
#include <shlobj_core.h>
#include <string>
#include "SketchTimerApp.h"
#include "Wnd.h"
#include "winFuncitons.h"
#include "resource.h"
#pragma optimize( "", off )
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
		SetWindowPos(mainWnd.GetHWnd(), HWND_TOPMOST, 0, 0, 100, 100, NULL);

	sta.SetHWnd(mainWnd.GetHWnd());

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
	HDC hDC;
	PAINTSTRUCT	ps;
	static RECT textRect;


	switch (uMsg)
	{
	case UM_SHOW_TIME:
		GetClientRect(hWnd, &textRect);
		time = std::to_wstring((int)wParam);
		InvalidateRect(hWnd, &textRect, TRUE);
		break;

	case WM_PAINT:
		hDC = BeginPaint(hWnd, &ps);
		TextOutW(hDC, 0, 0, time.c_str(), time.size());
		EndPaint(hWnd, &ps);
		break;

	case WM_DESTROY:
		PostQuitMessage(0);
		break;

	default:
		return DefWindowProcW(hWnd, uMsg, wParam, lParam);
	}
	return 0;
}