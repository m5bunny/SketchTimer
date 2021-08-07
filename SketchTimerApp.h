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
};


class SketchTimerApp
{
	std::wstring sourceDir;
	std::wstring selectedPict;
	std::vector<std::wstring> pictNames;
	int timeForPict;
	int numPict;
	bool isOverlay;
	HWND hWnd;

	void SelectPict();
	void FindPicts();
	void Idle();
public:
	void Initialize(SketchTimerInitData & initData);
	bool IsOverlay() const { return isOverlay; }
	const std::wstring & GetSelectPict() const;
	void ShowPict() const;
	void ShowTime(int t) const;
	void Start();
	void SetHWnd(const HWND & _hWnd) { hWnd = _hWnd; }
};

#endif // !SKETCHTIMERAPP_H_
