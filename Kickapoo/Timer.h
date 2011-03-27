#pragma once
#include "Common.h"

const int MAX_PROBES_AVG = 10;

class Timer : public Singleton<Timer>
{
public:
	Timer() : avgFps(100), m_engineTime(0.0f) { init(); }
	~Timer() {}

	float update();		
	float getFrameTime() { 
		return frameTime; 
	}

	float getEngineTime() {
		return m_engineTime;
	}

	float getFPS();
	float getAvgFPS() { return avgFps; }

	LARGE_INTEGER getCurrentTime();
	double getCurrentTimeF();
	double calculateTime(LARGE_INTEGER *time);

private:
	LARGE_INTEGER startTime;
	LARGE_INTEGER ticksPerSecond;		

	LARGE_INTEGER startTimer;
	
	float frameTime;
	float avgFps;
	float m_engineTime;

	void init();
};
inline Timer* g_Timer() { 
	return Timer::getSingletonPtr(); 
}