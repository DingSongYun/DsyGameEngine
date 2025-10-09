#pragma once

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
