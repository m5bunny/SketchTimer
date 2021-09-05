#include <Windows.h>
#include <shlobj_core.h>
#include <string>
#include <gdiplus.h>
#include "SketchTimerApp.h"
#include "Wnd.h"
#include "winFuncitons.h"
#include "resource.h"
#include <fstream>
#include <locale> 
#include <algorithm>

std::locale mylocale("");

SketchTimerInitData initData;
SketchTimerPreset presetData;
SketchTimerApp sta;

BOOL CALLBACK savePresetTitleDlgProc(HWND, UINT, WPARAM, LPARAM);
BOOL CALLBACK initDataDlgProc(HWND, UINT, WPARAM, LPARAM);
LRESULT CALLBACK WndProc(HWND, UINT, WPARAM, LPARAM);
void SaveSettings(const HWND hWnd);
void SavePreset(const std::wstring & settingsFileTitle, SketchTimerPreset& _presetData);

int WINAPI wWinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPWSTR lpCmdLine, int nCmdShow)
{
	MSG msg; 
	Gdiplus::GdiplusStartupInput si;
	ULONG_PTR token;

	Wnd mainWnd(L"Sketch Timer", hInstance, nCmdShow, WndProc, NULL, sta.GetInitWndSize()[0], sta.GetInitWndSize()[1], sta.GetInitWndSize()[2], sta.GetInitWndSize()[3]);

	DialogBoxW(hInstance, MAKEINTRESOURCEW(IDD_INIT_DATA), mainWnd.GetHWnd(), initDataDlgProc);

	Gdiplus::GdiplusStartup(&token, &si, 0);
	if (sta.IsOverlay())
		SetWindowPos(mainWnd.GetHWnd(), HWND_TOPMOST, 0, 0, 0, 0, SWP_NOMOVE | SWP_NOREDRAW | SWP_NOSENDCHANGING | SWP_NOSIZE);
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

BOOL CALLBACK savePresetTitleDlgProc(HWND hDlg, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	switch (uMsg)
	{
	case WM_INITDIALOG:
		return TRUE;
	case WM_COMMAND:
		switch (LOWORD(wParam))
		{
		case IDOK:
		{
			wchar_t titleBuf[100];
			SendDlgItemMessageW(hDlg, IDD_M_PRESET_TITLE, WM_GETTEXT, 100, (LPARAM)titleBuf);
			presetData._presetTitle = titleBuf;
			if (presetData._presetTitle.length() > 0)
				EndDialog(hDlg, TRUE);
			else
				MessageBoxW(hDlg, L"You have to enter the title to save the preset", L"Error", MB_OK | MB_ICONERROR);
			return TRUE;
		}
		case IDCANCEL:
			EndDialog(hDlg, FALSE);
			return TRUE;
		}
		break;
	}

	return FALSE;
}

BOOL CALLBACK initDataDlgProc(HWND hDlg, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	static BROWSEINFOW  bi{};
	static wchar_t text[100]{};
	static HWND hSelectPresetCB;
	static std::vector<std::wstring> presetList;
	switch (uMsg)
	{
	case WM_INITDIALOG:
	{
		bi.hwndOwner = hDlg;
		bi.lpszTitle = L"Chouse folder:";
		bi.pszDisplayName = text;

		hSelectPresetCB = GetDlgItem(hDlg, IDD_M_PRESETS_CB);
		std::wifstream settingsIn(sta.GetSettingFileTitle());
		settingsIn.imbue(mylocale);
		std::wstring buf;
		while (!settingsIn.eof())
		{
			std::getline(settingsIn, buf);
			if (buf == L"PRESET")
			{
				std::getline(settingsIn, buf);
				presetList.push_back(buf);
				SendMessageW(hSelectPresetCB, CB_ADDSTRING, 0, (LPARAM)buf.c_str());
			}
		}
		std::sort(presetList.begin(), presetList.end());
		if (presetList.size() == 0)
			EnableWindow(hSelectPresetCB, FALSE);

		settingsIn.close();
		return TRUE;
	}
	case WM_COMMAND:
		switch (LOWORD(wParam))
		{
		case IDD_M_SELECT_FOLDER:
		{
			LPITEMIDLIST lpItem = SHBrowseForFolderW(&bi);
			if (lpItem)
			{
				SHGetPathFromIDListW(lpItem, text);
				SendDlgItemMessageW(hDlg, IDD_M_SELECT_FOLDER_EB, WM_SETTEXT, 0, (LPARAM)text);
			}
		}
		break;

		case IDOK:
			SendDlgItemMessageW(hDlg, IDD_M_SELECT_FOLDER_EB, WM_GETTEXT, 100, (LPARAM)text);
			initData._sourceDir = text;
			SendDlgItemMessageW(hDlg, IDD_M_NUM_PICTS, WM_GETTEXT, 100, (LPARAM)text);
			initData._numPict = _wtoi(text);
			SendDlgItemMessageW(hDlg, IDD_M_TIME_FOR_PIC, WM_GETTEXT, 100, (LPARAM)text);
			initData._timeForPict = _wtoi(text);
			initData._isOverlay = (IsDlgButtonChecked(hDlg, IDD_M_IS_OVERLAYED) == BST_CHECKED) ? true : false;
			initData._isAutosizing = (IsDlgButtonChecked(hDlg, IDD_M_IS_AUTOSIZE) == BST_CHECKED) ? true : false;
			if (initData._sourceDir != L"" && initData._numPict > 0 && initData._timeForPict > 0)
			{
				sta.Initialize(initData);
				EndDialog(hDlg, TRUE);
			}
			else
				MessageBoxW(hDlg, L"The path can't be blank, number of pictures and time for picture should be more than 0", L"Error", MB_ICONERROR | MB_OK);
			return TRUE;
		
		case IDD_M_PRESETS_CB:
		{
			std::wifstream settingsIn(sta.GetSettingFileTitle());
			settingsIn.imbue(mylocale);
			std::wstring buf;
			int selectedPreset =SendMessageW(hSelectPresetCB, CB_GETCURSEL, 0, 0);
			if (selectedPreset >= 0)
			{

				while (!settingsIn.eof())
				{
					std::getline(settingsIn, buf);
					if (buf == presetList[selectedPreset])
					{
						std::getline(settingsIn, buf);
						SendDlgItemMessageW(hDlg, IDD_M_SELECT_FOLDER_EB, WM_SETTEXT, 0, (LPARAM)buf.c_str());
						std::getline(settingsIn, buf);
						SendDlgItemMessageW(hDlg, IDD_M_NUM_PICTS, WM_SETTEXT, 0, (LPARAM)buf.c_str());
						std::getline(settingsIn, buf);
						SendDlgItemMessageW(hDlg, IDD_M_TIME_FOR_PIC, WM_SETTEXT, 0, (LPARAM)buf.c_str());
						std::getline(settingsIn, buf);
						if (buf == L"1") CheckDlgButton(hDlg, IDD_M_IS_OVERLAYED, BST_CHECKED);
						else CheckDlgButton(hDlg, IDD_M_IS_OVERLAYED, BST_UNCHECKED);
						std::getline(settingsIn, buf);
						if (buf == L"1") CheckDlgButton(hDlg, IDD_M_IS_AUTOSIZE, BST_CHECKED);
						else CheckDlgButton(hDlg, IDD_M_IS_AUTOSIZE, BST_UNCHECKED);
						break;
					}
				}
			}
			settingsIn.close();
		}
		break;

		case IDD_M_SAVE_PRESET:
			DialogBoxW(GetModuleHandleW(NULL), MAKEINTRESOURCE(IDD_SAVE_PRESET_TITLE), hDlg, savePresetTitleDlgProc);
			if (presetData._presetTitle == L"")
				break;
			SendDlgItemMessageW(hDlg, IDD_M_SELECT_FOLDER_EB, WM_GETTEXT, 100, (LPARAM)text);
			presetData._sourceDir = text;
			SendDlgItemMessageW(hDlg, IDD_M_NUM_PICTS, WM_GETTEXT, 100, (LPARAM)text);
			presetData._numPict = _wtoi(text);
			SendDlgItemMessageW(hDlg, IDD_M_TIME_FOR_PIC, WM_GETTEXT, 100, (LPARAM)text);
			presetData._timeForPict = _wtoi(text);
			presetData._isOverlay = (IsDlgButtonChecked(hDlg, IDD_M_IS_OVERLAYED) == BST_CHECKED) ? true : false;
			presetData._isAutosizing = (IsDlgButtonChecked(hDlg, IDD_M_IS_AUTOSIZE) == BST_CHECKED) ? true : false;
			if (presetData._sourceDir.length() > 0 && presetData._numPict > 0 && presetData._timeForPict > 0)
			{
				SavePreset(sta.GetSettingFileTitle(), presetData);
				presetList.push_back(presetData._presetTitle);
				std::sort(presetList.begin(), presetList.end());
				SendMessageW(hSelectPresetCB, CB_ADDSTRING, 0, (LPARAM)presetData._presetTitle.c_str());
				EnableWindow(hSelectPresetCB, TRUE);
			}
			else
				MessageBoxW(hDlg, L"The path can't be blank, number of pictures and time for picture should be more than 0", L"Error", MB_ICONERROR | MB_OK);
			break;

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

	static int prevTimeRectLeft{};

	switch (uMsg)
	{
	case WM_CREATE:
		if (sta.IsOverlay())
			overlay = HWND_TOPMOST;
		
		break;

	case UM_SHOW_TIME:
		GetClientRect(hWnd, &pictRect);
		time = std::to_wstring((int)wParam);
		lstrcpyW((LPWSTR)&lf.lfFaceName, L"Lucida Console");
		lf.lfHeight = 20;
		lf.lfWidth = 20;
		hFont = CreateFontIndirectW(&lf);
		if (prevTimeRectLeft == 0)
			prevTimeRectLeft = pictRect.right - (time.length() * lf.lfWidth);

		timeRect.left = prevTimeRectLeft;
		timeRect.right = pictRect.right;
		timeRect.bottom = lf.lfHeight;
		timeRect.top = 0;
		InvalidateRect(hWnd, &timeRect, TRUE);
		prevTimeRectLeft = pictRect.right - (time.length() * lf.lfWidth);
		break;

	case UM_SHOW_PICT:
		pictName = (wchar_t*)wParam;
		pict = Gdiplus::Image::FromFile(pictName);
		GetClientRect(hWnd, &pictRect);

		widthToHeightCoef = (double)pict->GetHeight() / (double)pict->GetWidth();
		if (sta.IsAutosizing())
		{
			pictHeight = pictWidth * widthToHeightCoef;
			windowWidth = pictWidth + pictToWndAddW;
			windowHeight = pictHeight + pictToWndAddH;
		}
		else
		{
			if (pictRect.bottom >= (pictRect.right * widthToHeightCoef))
			{
				pictWidth = pictRect.right;
				pictHeight = pictWidth * widthToHeightCoef;
			}
			else
			{
				pictHeight = pictRect.bottom;
				pictWidth = pictHeight / widthToHeightCoef;
			}
		}

		if (sta.IsAutosizing())
			SetWindowPos(hWnd, overlay, 0, 0, windowWidth, windowHeight, SWP_NOMOVE | SWP_NOREDRAW | SWP_NOSENDCHANGING);

		SendMessageW(hWnd, UM_UPDATE_PICT, 0, 0);
		break;

	case UM_UPDATE_PICT:
	{
		GetWindowRect(hWnd, &rc);
		GetClientRect(hWnd, &pictRect);
		if (sta.IsAutosizing())
		{
			pictToWndAddW = (rc.right - rc.left) - (pictRect.right - pictRect.left);
			pictToWndAddH = (rc.bottom - rc.top) - (pictRect.bottom - pictRect.top);
		}
		hDC = GetDC(hWnd);
		hMemDC = CreateCompatibleDC(hDC);
		hMemBmp = CreateCompatibleBitmap(hDC, pictRect.right, pictRect.bottom);
		SelectObject(hMemDC, hMemBmp);
		Gdiplus::Graphics grap(hMemDC);
		if (sta.IsAutosizing())
			grap.DrawImage(pict, 0, 0, pictWidth, pictHeight);
		else
		{
			int x = (double)(pictRect.right - pictWidth) / 2.;
			int y = (double)(pictRect.bottom - pictHeight) / 2.;
			grap.DrawImage(pict, x, y, pictWidth, pictHeight);
		}
		InvalidateRect(hWnd, NULL, TRUE);
	}
	break;

	case WM_SIZE:
		if (sta.IsAutosizing())
		{
			pictWidth = (LOWORD(lParam) > 0) ? LOWORD(lParam) : pictWidth;
			pictHeight = pictWidth * widthToHeightCoef;
			windowWidth = pictWidth + pictToWndAddW;
			windowHeight = pictHeight + pictToWndAddH;
		}
		else
		{
			GetClientRect(hWnd, &pictRect);
			if (pictRect.bottom >= (pictRect.right * widthToHeightCoef))
			{
				pictWidth = pictRect.right;
				pictHeight = pictWidth * widthToHeightCoef;
			}
			else
			{
				pictHeight = pictRect.bottom;
				pictWidth = pictHeight / widthToHeightCoef;
			}
		}
		SendMessageW(hWnd, UM_UPDATE_PICT, 0, 0);
		break;

	case WM_GETMINMAXINFO:
	{
		if (sta.IsAutosizing())
		{
			LPMINMAXINFO lpMMI = (LPMINMAXINFO)lParam;
			lpMMI->ptMinTrackSize.y = lpMMI->ptMaxTrackSize.y = windowHeight;
		}
		else DefWindowProc(hWnd, uMsg, wParam, lParam);
	}

	case WM_PAINT:
		hDC = BeginPaint(hWnd, &ps);
		GetClientRect(hWnd, &pictRect);
		BitBlt(hDC, 0, 0, pictRect.right, pictRect.bottom, hMemDC, 0, 0, SRCCOPY);
		hFontOld = (HFONT)SelectObject(hDC, hFont);
		timeRect.left = pictRect.right - (time.length() * lf.lfWidth);
		TextOutW(hDC, timeRect.left + 2, 0, time.c_str(), time.size());
		SelectObject(hDC, hFontOld);
		EndPaint(hWnd, &ps);
		break;

	case WM_ERASEBKGND:
		if (sta.IsAutosizing())
			return 0;
		else
			DefWindowProcW(hWnd, uMsg, wParam, lParam);
		break;

	case WM_KEYDOWN:
		if (wParam == VK_SPACE)
		{
			GetClientRect(hWnd, &pictRect);
			sta.ChangePaused();
			time = L"Paused";
			prevTimeRectLeft = pictRect.right - (time.length() * lf.lfWidth);
			timeRect.left = prevTimeRectLeft;
			timeRect.right = pictRect.right;
			timeRect.bottom = lf.lfHeight;
			timeRect.top = 0;
			InvalidateRect(hWnd, &timeRect, TRUE);
		}
		if (wParam == VK_RIGHT)
			sta.SkipPict();
		break;

	case WM_DESTROY:
		SaveSettings(hWnd);
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

void SaveSettings(const HWND hWnd)
{
	int counter{};
	bool isUserPresetFlag{ false };
	std::wfstream settingsIn(sta.GetSettingFileTitle());
	settingsIn.imbue(mylocale);
	std::wstring buf;
	while (!settingsIn.eof())
	{
		std::getline(settingsIn, buf);
		if (buf == L"PRESET")
			++counter;
		if (counter == 2)
		{
			isUserPresetFlag = true;
			break;
		}
	}
	if (isUserPresetFlag)
	{
		std::wstring singleStrbuf;
		buf.clear();
		buf = L"PRESET\n";
		while (!settingsIn.eof())
		{
			std::getline(settingsIn, singleStrbuf);
			if (!settingsIn.eof())
				buf += singleStrbuf + L"\n";
		}
	}

	settingsIn.close();


	std::wofstream settingsOut(sta.GetSettingFileTitle());
	settingsOut.imbue(mylocale);
	RECT currentWndRect;
	GetWindowRect(hWnd, &currentWndRect);
	settingsOut << currentWndRect.left << std::endl
		<< currentWndRect.top << std::endl
		<< (currentWndRect.right - currentWndRect.left) << std::endl
		<< (currentWndRect.bottom - currentWndRect.top) << std::endl;
	settingsOut.close();
	presetData._presetTitle = L"Last session";
	presetData._sourceDir = initData._sourceDir;
	presetData._numPict = initData._numPict;
	presetData._timeForPict = initData._timeForPict;
	presetData._isOverlay = initData._isOverlay;
	presetData._isAutosizing = initData._isAutosizing;
	SavePreset(sta.GetSettingFileTitle(), presetData);
	if (isUserPresetFlag)
	{
		settingsOut.open(sta.GetSettingFileTitle(), std::ios::app);
		settingsOut << buf;
		settingsOut.close();
	}
}

void SavePreset(const std::wstring & settingsFileTitle,SketchTimerPreset & _presetData)
{
	std::wofstream settingsOut(sta.GetSettingFileTitle(), std::ios_base::app);
	settingsOut.imbue(mylocale);
	settingsOut << L"PRESET" << std::endl
				<< _presetData._presetTitle.c_str() << std::endl
				<< _presetData._sourceDir.c_str() << std::endl
				<< _presetData._numPict << std::endl
				<< _presetData._timeForPict << std::endl
				<< _presetData._isOverlay << std::endl
				<< _presetData._isAutosizing << std::endl;
	settingsOut.close();
}