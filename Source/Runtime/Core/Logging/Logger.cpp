#include "Logger.h"
#include "LogSink.h"

Logger::Logger()
{
	AddSink<LogConsoleSink>();
}

Logger& Logger::GetInstance()
{
	static Logger sLogger;
	return sLogger;
}
