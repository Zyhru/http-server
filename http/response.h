#pragma once

#include <iostream>
#include <filesystem>
#include <fstream>
#include <cstring>
#include <sys/socket.h>

#include "../log/logger.h"

namespace fs = std::filesystem;

// Parse request and set the html file to that and then check

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
	std::string html_file;
	std::string error_html = "html/error.html";

	void sendResponse(int cd);

private:
	char *buffer;
	bool htmlFileExists();
	void readHTML(std::string& path);
	void countHTML();
	std::string constructResponse();
};


inline bool Response::htmlFileExists() {
	const fs::path html_path{"html/"};
	for(auto& file : fs::directory_iterator(html_path)) { 
		std::string file_name =  file.path().string();
		size_t found = file_name.find(html_file);
		if(found != std::string::npos) {
			return true;
		}
	}
	return false;
}

inline void Response::readHTML(std::string& path) {
	std::fstream file(path);
	if(!file.is_open()) {
		Logger::Log(Logger::ERROR, "Unable to open file at: %s", path.c_str());
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

inline void Response::sendResponse(int cd) {
	Logger::Log(Logger::INFO, "Preparing to send response %s", html_file.c_str());
	if(htmlFileExists()) {
		header.status_code = 200;
		header.message = "OK"; 
		readHTML(html_file);
	} else {
		header.status_code = 404;
		header.message = "ERROR"; 
		readHTML(error_html);
	}

	header.content_type = "text/html"; // hard coded for now. handle .js later
	countHTML();

	std::string response_message = constructResponse();
	size_t response = send(cd, response_message.c_str(), strlen(response_message.c_str()), 0);
	int error_code = -1;
	if(response == error_code) {
		Logger::Log(Logger::ERROR, "Unable to send a request to server.");
		return;
	}
}

inline std::string Response::constructResponse() {
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
