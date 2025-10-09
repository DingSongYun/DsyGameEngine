#pragma once

#include <string>
#include <concepts>
#include <format>
#include <chrono>

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

/** 主日志器类 */
class Logger {
	Logger();
public:
	inline void SetLevel(LogLevel level) { m_level = level; }

	template<typename... Args>
	void Log(LogLevel level, const char* format, Args&&... args);

	template<LogSinkType TSink, typename... Args>
	void AddSink(Args&&... args)
	{
		m_sinks.push_back(std::make_shared<TSink>(std::forward<Args>(args)...));
	}

	template<LogFormatter TFormatter, typename... Args>  
	void SetFormatter(Args&&... args)
	{
		m_formatter = std::make_shared<TFormatter>(std::forward<Args>(args)...);
	}

	static Logger& GetInstance();
private:
	LogLevel m_level = LogLevel::INFO;
	std::vector<std::shared_ptr<ILogSink>> m_sinks;
	std::shared_ptr<ILogFormatter> m_formatter = nullptr;
};

/** 日志输出接口 */
class ILogSink {
public:
    virtual ~ILogSink() = default;
    virtual void Write(LogLevel level, const std::string& message) = 0;
};

/** 日志格式化器接口 */
class ILogFormatter {
public:
    virtual ~ILogFormatter() = default;
    virtual std::string Format(LogLevel level, const std::string& message, 
                              const std::chrono::system_clock::time_point& timestamp) = 0;
};

template<typename... Args>
void Logger::Log(LogLevel level, const char* format, Args&&... args)
{
	// 1. Level check
	if (level < m_level)
	{
		return;
	}

	// 2. Format
	auto timestamp = std::chrono::system_clock::now();
	std::string message;
	try 
	{
		message = std::vformat(format, std::make_format_args(args...));
		//message = std::format(format, std::forward<Args>(args)...);
		if (m_formatter) message = m_formatter->Format(level, message, timestamp);
	}
	catch (...)
	{
		message = format;
	}

	// 3. Write
	for (auto sink : m_sinks) 
	{
		if (sink)
		{
			sink->Write(level, message);
		}
	}
}

#define LOG(level, format, ...)		Logger::GetInstance().Log(level, format, __VA_ARGS__)
#define LOG_TRACE(format, ...)		LOG(LogLevel::TRACE, format, __VA_ARGS__)
#define LOG_INFO(format, ...)		LOG(LogLevel::INFO, format, __VA_ARGS__)
#define LOG_DEBUG(format, ...)		LOG(LogLevel::DEBUG, format, __VA_ARGS__)
#define LOG_WARN(format, ...)		LOG(LogLevel::WARN, format, __VA_ARGS__)
#define LOG_ERROR(format, ...)		LOG(LogLevel::ERROR, format, __VA_ARGS__)
#define LOG_FATAL(format, ...)		LOG(LogLevel::FATAL, format, __VA_ARGS__)
