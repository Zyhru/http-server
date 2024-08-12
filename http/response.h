#pragma once

#include <iostream>
#include <filesystem>
#include <fstream>
#include <cstring>
#include <sys/socket.h>

#include "../log/logger.h"


typedef struct ResponseHeader {
	int status_code;
	std::string message;
	std::string content_type;
	int content_length;
	std::string body;
} ResponseHeader;

class Response {
public:
	ResponseHeader header;
	std::string url_path;
	std::string constructResponse();

	Response() {
		header.content_type = "text/html";
	}

private:
	void readHTML(const std::string& path);
	void countHTML();
};


inline void Response::readHTML(const std::string& path) {
	std::fstream file(path);
	if(!file.is_open()) {
		Logger::Log(Logger::ERROR, "Unable to open file at: %s", path.c_str());
		header.status_code = 404;
		return;
	}

	std::string line;
	std::string html_data;
	while(std::getline(file, line)) {
		html_data += line;
	}

	header.body = html_data;
	file.close();
}

inline std::string Response::constructResponse() {
	if(url_path != "/") {
		header.status_code = 404;
		header.message = "ERROR"; 
		readHTML("html/error.html");
	} else {
		header.status_code = 200;
		header.message = "OK"; 
		readHTML("html/about.html");
	}
	
	countHTML();

	std::string response_header = "HTTP/1.1 ";
	response_header += std::to_string(header.status_code) + "\r\n";
	response_header += "Content-Type: " + header.content_type + "\r\n";
	response_header += "Content-Length: " + std::to_string(header.content_length) + "\r\n";
	response_header += "\r\n";
	response_header += header.body + "\r\n";
	Logger::Log(Logger::INFO, "Response Header: \n%s", response_header.c_str());
	return response_header;
}

inline void Response::countHTML() {
	int size;
	for(auto& data : header.body) {
		size++;
	}

	header.content_length = size;
}
