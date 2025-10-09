#pragma once
#include <string>
#include <iostream>
#include "Logger.h"

#ifdef _WIN32
#include <windows.h>
#endif

// 日志输出接口
class LogConsoleSink: public ILogSink 
{
private:
	WORD m_originalAttributes = 7; // 保存原始颜色属性

public:
	LogConsoleSink()
	{
#ifdef _WIN32
		// 初始化, 保存当前控制台的颜色属性
		HANDLE hConsole = GetStdHandle(STD_OUTPUT_HANDLE);
		if (hConsole != INVALID_HANDLE_VALUE)
		{
			CONSOLE_SCREEN_BUFFER_INFO consoleInfo;
			if (GetConsoleScreenBufferInfo(hConsole, &consoleInfo))
			{
				m_originalAttributes = consoleInfo.wAttributes;
			}
		}
#endif
	}

	virtual ~LogConsoleSink() = default;

	virtual void Write(LogLevel level, const std::string& message)
	{
		// 根据日志级别设置颜色
		SetConsoleColor(level);
		std::cout << message << std::endl;
		ResetConsoleColor();
	}

private:
	void SetConsoleColor(LogLevel level)
	{
#ifdef _WIN32
		HANDLE hConsole = GetStdHandle(STD_OUTPUT_HANDLE);
		switch (level) 
		{
		case LogLevel::LL_TRACE: SetConsoleTextAttribute(hConsole, 8); break;  // 灰色
		case LogLevel::LL_DEBUG: SetConsoleTextAttribute(hConsole, 7); break;  // 白色
		case LogLevel::LL_INFO:  SetConsoleTextAttribute(hConsole, 10); break; // 绿色
		case LogLevel::LL_WARN:  SetConsoleTextAttribute(hConsole, 14); break; // 黄色
		case LogLevel::LL_ERROR: SetConsoleTextAttribute(hConsole, 12); break; // 红色
		case LogLevel::LL_FATAL: SetConsoleTextAttribute(hConsole, 13); break; // 紫色
		}
#endif
	}

	void ResetConsoleColor()
	{
#ifdef _WIN32
		HANDLE hConsole = GetStdHandle(STD_OUTPUT_HANDLE);
		SetConsoleTextAttribute(hConsole, m_originalAttributes); // 默认白色
#endif
	}
};

//class FileSink : public ILogSink {
//public:
//	FileSink(const std::string& filename);
//	~FileSink();
//
//	void Write(LogLevel level, const std::string& message) override {
//		if (m_file.is_open()) {
//			m_file << message << std::endl;
//			m_file.flush(); // 确保立即写入
//		}
//	}
//
//private:
//	std::ofstream m_file;
//	std::mutex m_mutex; // 线程安全
//};