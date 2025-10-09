#pragma once

#include <string>
#include <concepts>

// 日志级别枚举
enum class LogLevel {
	TRACE = 0,
	DEBUG = 1,
	INFO = 2,
	WARN = 3,
	ERROR = 4,
	FATAL = 5
};

class ILogSink;
class ILogFormatter;
template<typename T> concept LogSinkType = std::derived_from<T, ILogSink>;
template<typename T> concept LogFormatter = std::derived_from<T, ILogFormatter>;

// 主日志器类
class Logger {
public:
    template<typename... Args>
    void Log(LogLevel level, const std::string& format, Args&&... args);
    void SetLevel(LogLevel level);
    
	template<LogSinkType TSink, typename... Args>
	void SetSink(Args&&... args);

	template<LogFormatter TFormatter, typename... Args>  
	void SetFormatter(Args&&... args);

private:
	LogLevel m_level = LogLevel::INFO;
	ILogSink* m_sink = nullptr;
	ILogFormatter* m_formatter = nullptr;
};

// 日志输出接口
class ILogSink {
public:
    virtual ~ILogSink() = default;
    virtual void Write(LogLevel level, const std::string& message) = 0;
};

// 日志格式化器接口
class ILogFormatter {
public:
    virtual ~ILogFormatter() = default;
    virtual std::string Format(LogLevel level, const std::string& message, 
                              const std::chrono::system_clock::time_point& timestamp) = 0;
};

template<typename... Args>
void Logger::Log(LogLevel level, const std::string& format, Args&&... args)
{
	// 1. Level check
	if (level < m_level)
	{
		return;
	}
}
