#pragma once

#include <memory>
#include <sys/socket.h>

#include "http/request.h"
#include "http/response.h"
#include "log/logger.h"

class Buffer {
public:
	Buffer(int capacity) : _buffer(std::make_unique<char[]>(capacity)), _capacity(capacity) {};
	
	void send_data(int cd, Response& res);
	void recv_data(int cd, Request& req);

private:
	std::unique_ptr<char[]> _buffer;
	int _capacity;
	size_t _fill_count;

	void reset_buffer() { 
		std::memset(_buffer.get(), 0, _fill_count);
		_fill_count = 0;
	}
};

inline void Buffer::send_data(int cd, Response& res) {
	std::string response = res.constructResponse();
	size_t size = response.size();
	
	if(_fill_count == 0) {
		_fill_count = size;
		for(size_t i = 0; i < size; ++i) {
			_buffer.get()[i] = response[i];
		}

	}

	Logger::Log(Logger::INFO, "Response Data: \n%s", _buffer.get());
	size_t bytes = send(cd, _buffer.get(), strlen(_buffer.get()), 0);
	if(bytes == -1) {
		Logger::Log(Logger::ERROR, "failed to send data.");
		return;
	}

	reset_buffer();
}

inline void Buffer::recv_data(int cd, Request& res) {
	_fill_count = recv(cd, _buffer.get(), _capacity, 0);
	if(_fill_count < 0) {
		Logger::Log(Logger::ERROR, "failed to recieve data.");
		return;
	}
	
	Logger::Log(Logger::INFO, "Request Data: \nbytes recv: %d\n%s", _fill_count, _buffer.get());
	res.parseRequestHeader(_buffer.get());
	reset_buffer();
}
