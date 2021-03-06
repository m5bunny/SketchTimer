#ifndef SKETCHTIMERAPP_H_
#define SKETCHTIMERAPP_H_
#include <windows.h>
#include <iostream>
#include <string>
#include <vector>
#include <random>
#include <ctime>

struct SketchTimerInitData
{
	std::wstring _sourceDir;
	int _timeForPict;
	int _numPict;
	bool _isOverlay;
	bool _isAutosizing;
};

struct SketchTimerPreset
{
	std::wstring _presetTitle;
	std::wstring _sourceDir;
	int _timeForPict;
	int _numPict;
	bool _isOverlay;
	bool _isAutosizing;
};

class SketchTimerApp
{
	std::wstring settingsFileTitle = L"Settings.txt";
	int initWndSize[4];

	std::wstring sourceDir;
	std::wstring selectedPict;
	std::vector<std::wstring> pictNames;
	int timeForPict;
	int numPict;
	bool isOverlay;
	bool isPaused;
	bool isSkiping;
	bool isAutosizing;
	HWND hWnd;

	void SelectPict();
	void FindPicts();
	void Idle();
	void ReedInitWndSize();
public:
	SketchTimerApp();
	void Initialize(SketchTimerInitData& initData);
	bool IsOverlay() const { return isOverlay; }
	bool IsAutosizing() const { return isAutosizing; }
	const std::wstring& GetSelectPict() const;
	void ShowPict() const;
	void ShowTime(int t) const;
	void Start();
	void SetHWnd(const HWND& _hWnd) { hWnd = _hWnd; }
	void ChangePaused() { isPaused = !isPaused;};
	void SkipPict() { isSkiping = true; }
	int * GetInitWndSize() { return initWndSize;  };
	const std::wstring& GetSettingFileTitle() const { return settingsFileTitle; }
};

#endif // !SKETCHTIMERAPP_H_
