#pragma once
#include <string>
#include <iostream>
#include "Logger.h"

#ifdef _WIN32
#include <windows.h>
#endif

// ��־����ӿ�
class LogConsoleSink: public ILogSink 
{
private:
	WORD m_originalAttributes = 7; // ����ԭʼ��ɫ����

public:
	LogConsoleSink()
	{
#ifdef _WIN32
		// ��ʼ��, ���浱ǰ����̨����ɫ����
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
		// ������־����������ɫ
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
		case LogLevel::LL_TRACE: SetConsoleTextAttribute(hConsole, 8); break;  // ��ɫ
		case LogLevel::LL_DEBUG: SetConsoleTextAttribute(hConsole, 7); break;  // ��ɫ
		case LogLevel::LL_INFO:  SetConsoleTextAttribute(hConsole, 10); break; // ��ɫ
		case LogLevel::LL_WARN:  SetConsoleTextAttribute(hConsole, 14); break; // ��ɫ
		case LogLevel::LL_ERROR: SetConsoleTextAttribute(hConsole, 12); break; // ��ɫ
		case LogLevel::LL_FATAL: SetConsoleTextAttribute(hConsole, 13); break; // ��ɫ
		}
#endif
	}

	void ResetConsoleColor()
	{
#ifdef _WIN32
		HANDLE hConsole = GetStdHandle(STD_OUTPUT_HANDLE);
		SetConsoleTextAttribute(hConsole, m_originalAttributes); // Ĭ�ϰ�ɫ
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
//			m_file.flush(); // ȷ������д��
//		}
//	}
//
//private:
//	std::ofstream m_file;
//	std::mutex m_mutex; // �̰߳�ȫ
//};