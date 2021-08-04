#include "SketchTimerApp.h"
#include "Timer.h"
#include "winFuncitons.h"

void SketchTimerApp::Initialize(SketchTimerInitData & initData)
{
	sourceDir = initData._sourceDir;
	timeForPict = initData._timeForPict;
	numPict = initData._numPict;
	isOverlay = true;
	FindPicts();
}


void SketchTimerApp::FindPicts()
{
	pictNames = wnd::FindPicts(sourceDir);
}

void SketchTimerApp::SelectPict()
{
	std::mt19937 gen(time(0));
	std::uniform_int_distribution<> uid(0, pictNames.size());
	selectedPict = pictNames[uid(gen)];
}

const std::wstring & SketchTimerApp::GetSelectPict() const
{
	return selectedPict;
}

void SketchTimerApp::ShowTime(int t) const
{
	wnd::ShowTime(t, hWnd);
}

void SketchTimerApp::Start()
{
	MSG msg;
	for (int i{}; i < numPict; ++i)
	{
		//SelectPict();
		//ShowPict();
		for (int j = timeForPict; j >= 0; --j)
		{
			ShowTime(j);
			Timer t;
			while (!t.IsDueDuration())
			{
				if (PeekMessage(&msg, hWnd, 0, 0, PM_REMOVE)) {
					TranslateMessage(&msg);
					DispatchMessage(&msg);
				}
			}
		}
	}
}
