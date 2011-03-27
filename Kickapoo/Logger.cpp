
#include "common.h"
#include "Logger.h"

CLogger::CLogger(void)
{
	m_file = fopen("log.txt", "w");
}

CLogger::~CLogger(void)
{
	fclose(m_file);
}
