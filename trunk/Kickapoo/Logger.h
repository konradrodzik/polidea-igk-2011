#pragma once
#include "Common.h"

class CLogger : public Singleton<CLogger>
{
public:
	CLogger(void);
	~CLogger(void);

	__forceinline void log(const char* format, ...) 
	{
		char buffer[1024];
		va_list argv;

		va_start(argv, format);
			_vstprintf( buffer, format, argv );
		va_end(argv);

		fwrite(buffer, 1, strlen(buffer), m_file);
		fflush(m_file);
	}

private:
	FILE* m_file;
	
};

DefineAccessToSingleton(CLogger);