#pragma once

#include <stdexcept>
#include <string>
#include <fstream>
#include "../log/logger.h"


class Util {
public:
	typedef enum ModeE {
		READ, WRITE

	} ModeE;
	std::fstream openFile(const std::string& file_name, ModeE mode);
};

inline std::fstream Util::openFile(const std::string& file_name, ModeE mode) {
	std::fstream file_stream;
	switch(mode) {
		case READ:
			file_stream.open(file_name, std::ios::in);
			break;
		case WRITE:
			file_stream.open(file_name, std::ios::out);
			break;
	}

	if(!file_stream.is_open()) {
		Logger::Log(Logger::ERROR, "Unable to open file %s", file_name.c_str());
		throw std::runtime_error("unable to open file");
	}
	
	return file_stream;
}
