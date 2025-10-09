#include "Logger.h"
#include "LogSink.h"

Logger::Logger()
{
	AddSink<LogConsoleSink>();
}

Logger& Logger::GetInstance()
{
	Logger sLogger;
	return sLogger;
}
