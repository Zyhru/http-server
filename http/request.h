#pragma once
#include <string>
#include <sys/socket.h>
#include <unistd.h>

#include "../log/logger.h"

class Request {
public:
	std::string method;
	std::string url;
	
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
