#ifndef WINFUNCTIONS_H_
#define WINFUNCTIONS_H_
#include <Windows.h>
#include <iostream>
#include <string>
#include <vector>
namespace wnd
{
#define UM_SHOW_TIME	WM_USER+1
#define UM_SHOW_PICT	WM_USER+2
#define UM_UPDATE_PICT	WM_USER+4


	bool isImage(const std::wstring title);

	std::vector<std::wstring> FindPicts(const std::wstring& path);

	void ShowTime(int t, const HWND & hWnd);

	void Idle(const HWND & hWnd);

	void ShowPict(const std::wstring & pictName, const HWND & hWnd);
}
#endif // !WINFUNCTIONS_H_


