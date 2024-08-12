#pragma once
#include <string>
#include <sys/socket.h>
#include <unistd.h>

#include "../log/logger.h"

class Request {
public:
	std::string method;
	std::string url;
	
	// void recvRequest(int cd); 
	void sendURL(std::string& url_s);
	void parseRequestHeader(const char* buffer);
	void handleRefresh();

};

inline void Request::handleRefresh() {
	method.clear();
	url.clear();
}

inline void Request::sendURL(std::string& url_s) {
	url_s.clear();
	url_s = url;
}

#if 0
inline void Request::recvRequest(int cd) {
	size_t request = recv(cd, request_header, sizeof(request_header), 0);
	if(request < 0) {
	Logger::Log(Logger::ERROR, "Recieved 0 bytes.");
		close(cd);
		return;
	};

	Logger::Log(Logger::INFO, "Request from client: \n%s", request_header);
	parseRequestHeader();
	Logger::Log(Logger::INFO, "Method: %s", method.c_str());
	Logger::Log(Logger::INFO, "URL Path: %s", url.c_str());
}
#endif

// GET 
inline void Request::parseRequestHeader(const char* buffer) {
	int spaces = 0;
	for(int i = 0; i < sizeof(buffer); ++i) {
		if(buffer[i] == ' ') ++spaces;

		// handling http method
		if(spaces == 0) {
			method += buffer[i];
		} 
	
		// handling url path
		if(spaces == 1) {
			if(buffer[i] == ' ') continue;
			url += buffer[i];
		}

		if(spaces == 2) break;
	}
}
