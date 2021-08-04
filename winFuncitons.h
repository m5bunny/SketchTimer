#ifndef WINFUNCTIONS_H_
#define WINFUNCTIONS_H_
#include <Windows.h>
#include <iostream>
#include <string>
#include <vector>
namespace wnd
{
#define UM_SHOW_TIME WM_USER+1


	bool isImage(const std::wstring title);

	std::vector<std::wstring> FindPicts(const std::wstring& path);

	void ShowTime(int t, HWND hWnd);
}
#endif // !WINFUNCTIONS_H_


