#pragma once
#include <string>
#include <sys/socket.h>
#include <unistd.h>

#include "../log/logger.h"

class Request {
public:
	std::string method;
	std::string url;
	
	void recvRequest(int cd); 
	void sendURL(std::string& url_s);
	void handleRefresh();

private:
	char request_header[4096];
	void parseRequestHeader();
};

inline void Request::handleRefresh() {
	method.clear();
	url.clear();
}

inline void Request::sendURL(std::string& url_s) {
	url_s.clear();
	url_s = url;
}


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

inline void Request::parseRequestHeader() {
	int spaces = 0;
	for(int i = 0; i < sizeof(request_header); i++) {
		if(request_header[i] == ' ') ++spaces;

		// handling http method
		if(spaces == 0) {
			method += request_header[i];
		} 
	
		// handling url path
		if(spaces == 1) {
			if(request_header[i] == ' ') continue;
			url += request_header[i];
		}

		if(spaces == 2) break;
	}
}
