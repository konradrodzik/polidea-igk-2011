#pragma once
#include "Common.h"

#define DefineAccessToSingleton(a) inline a* g_##a() { return a::getSingletonPtr(); }

inline DWORD FloatToDWORD( float f ) { return *((DWORD*)&f ); }

inline float RandomFloat(float min, float max)
{
	const float RANDOM_NUM = 1.0f / RAND_MAX;
	return min + (max - min) * RANDOM_NUM * (float)rand();
}