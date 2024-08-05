#pragma once
#include <curl/curl.h>
#include <stdlib.h>
#include <string.h>

#include "../log/logger.h"
#include "../json/single_include/nlohmann/json.hpp"

class OpenAI {

using json = nlohmann::json;

struct Memory {
	char *response;
	size_t size;
};

public:
	CURL *curl;
	struct Memory chunk;
	struct curl_slist *headers = NULL;
	const char *api_key;

	OpenAI(const char *env);
	~OpenAI();

	void sendCURL();

private:
	CURLcode res;
	std::string data;
	
	static size_t write_data(void *buffer, size_t size, size_t nmemb, void *userp) {
		size_t realsize = size * nmemb;
		struct Memory *mem = (struct Memory *)userp;

		char *ptr;
		ptr = (char *)realloc(mem->response, mem->size + realsize + 1);
		if(!ptr) {
		std::cout << "unable to reallocate memory";
		return 0;
		}

		mem->response = ptr;
		memcpy(&(mem->response[mem->size]), buffer, realsize);
		mem->size += realsize;
		mem->response[mem->size] = 0;
		return realsize;
	}

	void init();
	void setAPIKey(const char *api_key);
	void readPrompt(std::string& path);
};
