#include "Common.h"
#include "Timer.h"

void Timer::init()
{
	if (!QueryPerformanceFrequency(&ticksPerSecond))
		return;

	QueryPerformanceCounter(&startTime);
}

float Timer::update()
{
	static LARGE_INTEGER lastTime = startTime;
	LARGE_INTEGER currentTime;

	QueryPerformanceCounter(&currentTime);
	
	double seconds =  (static_cast<double>(currentTime.QuadPart) - static_cast<double>(lastTime.QuadPart)) 
				    / static_cast<double>(ticksPerSecond.QuadPart);
	
	lastTime = currentTime;	
	frameTime = seconds;

	m_engineTime += frameTime;
	return frameTime;
}

float Timer::getFPS()
{
	static LARGE_INTEGER lastTime = startTime;

	LARGE_INTEGER currentTime;

	QueryPerformanceCounter(&currentTime);

	float fps = static_cast<float>(ticksPerSecond.QuadPart) / (static_cast<float>(currentTime.QuadPart)
			  - static_cast<float>(lastTime.QuadPart));
	
	lastTime = currentTime;
	
	avgFps = (avgFps * 3 + fps) / 4;

	return avgFps;
}

LARGE_INTEGER Timer::getCurrentTime()
{
	LARGE_INTEGER currentTime;
	QueryPerformanceCounter(&currentTime);
	return currentTime;
}

double Timer::calculateTime(LARGE_INTEGER *time)
{
	LARGE_INTEGER currentTime;

	QueryPerformanceCounter(&currentTime);

	return (static_cast<double>(currentTime.QuadPart) - static_cast<double>(time->QuadPart)) 
		  / static_cast<double>(ticksPerSecond.QuadPart);
}