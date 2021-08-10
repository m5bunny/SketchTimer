#include "SketchTimerApp.h"
#include "Timer.h"
#include "winFuncitons.h"

void SketchTimerApp::Initialize(SketchTimerInitData & initData)
{
	sourceDir = initData._sourceDir;
	timeForPict = initData._timeForPict;
	numPict = initData._numPict;
	isOverlay = initData._isOverlay;
	isPaused = false;
	isSkiping = false;
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
	for (int i{}; i < numPict; ++i)
	{
		SelectPict();
		ShowPict();
		for (int j = timeForPict; j >= 0; --j)
		{
			ShowTime(j);
			Timer t;
			while (!t.IsDueDuration() || isPaused) Idle();
			if (isSkiping)
			{
				isSkiping = false;
				break;
			}
		}
	}
}

void SketchTimerApp::Idle()
{
	wnd::Idle(hWnd);
}

void SketchTimerApp::ShowPict() const
{
	wnd::ShowPict(selectedPict, hWnd);
}
