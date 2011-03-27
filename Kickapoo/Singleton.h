
#pragma once


template <typename T> class Singleton
{
public:
	static T& getSingleton() {
		static T obj;
		return obj;
	}

	static T* getSingletonPtr() {
		return &getSingleton();
	}
};