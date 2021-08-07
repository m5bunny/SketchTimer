#include "winFuncitons.h"

bool wnd::isImage(const std::wstring title)
{
	std::wstring filter[] = { L".jpg", L".jpeg", L".gif", L".png", L".webp", L".JPG", L".JPEG", L".GIF", L".PNG", L".WEBP" };
	for (int i{}; i < 10; ++i)
		if (title.find(filter[i]) != std::string::npos)
			return true;
	return false;
}

std::vector<std::wstring> wnd::FindPicts(const std::wstring & path)
{
	std::vector<std::wstring> fList{};

	WIN32_FIND_DATA findData;
	HANDLE hFind;

	std::wstring completePath, newDirPath;
	completePath = path + L"/*";

	hFind = FindFirstFileW(completePath.c_str(), &findData);
	if (hFind != INVALID_HANDLE_VALUE)
	{
		FindNextFileW(hFind, &findData);
		while (FindNextFileW(hFind, &findData))
		{
			if (isImage(findData.cFileName))
				fList.emplace_back(path + L"/" + findData.cFileName);
			else if (findData.dwFileAttributes == FILE_ATTRIBUTE_DIRECTORY)
			{
				std::to_wstring(findData.ftLastWriteTime.dwLowDateTime);
				std::wstring newDirPath = path + L"/" + findData.cFileName;
				std::vector<std::wstring> fNewDirFList{ wnd::FindPicts(newDirPath) };

				fList.insert(fList.end(), fNewDirFList.begin(), fNewDirFList.end());
			}
		}
		FindClose(hFind);
	}
	else
		MessageBoxW(NULL, L"There is no such a folder", L"Error", MB_OK | MB_ICONERROR);

	return fList;
}

void wnd::ShowTime(int t, const HWND & hWnd)
{
	SendMessageW(hWnd, UM_SHOW_TIME, (WPARAM)t, 0);
}

void wnd::Idle(const HWND & hWnd)
{
	MSG msg;
	if (PeekMessage(&msg, hWnd, 0, 0, PM_REMOVE)) 
	{
		TranslateMessage(&msg);
		DispatchMessage(&msg);
	}
}

void wnd::ShowPict(const std::wstring & pictName, const HWND & hWnd)
{
	SendMessageW(hWnd, UM_SHOW_PICT, (WPARAM)pictName.c_str(), 0);
}