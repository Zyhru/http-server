#pragma once
#include <iostream>
#include <cstdarg>
#include <stdio.h>

// TODO: std::format (C++ 20)
class Logger {
public: 
	enum ELevel {INFO, ERROR};
	static void Log(ELevel level, const char *fmt, ...);
};

inline void Logger::Log(ELevel level, const char *fmt, ...) {
	va_list l;
	va_start(l, fmt);

	switch(level) {
		case INFO:
			std::cout << "[INFO] ";
			vprintf(fmt, l);
			break;
		case ERROR:
			std::cout << "[ERROR] ";
			vprintf(fmt, l);
			break;
	}

	printf("\n");
	va_end(l);
}
