#pragma once
#include <curl/curl.h>
#include <stdlib.h>
#include <string.h>
#include <fstream>

#include "../log/logger.h"
#include "../util/util.h"
#include "../json/single_include/nlohmann/json.hpp"
#include "../json/include/nlohmann/json.hpp"

using json = nlohmann::json;

class OpenAI {

struct Memory {
	char *response;
	size_t size;
};

public:
	CURL *curl;
	Util u;
	
	struct Memory chunk;
	struct curl_slist *headers = NULL;
	const char *api_key;
	std::string json_data;

	OpenAI();
	~OpenAI();

	void sendCURL();

private:
	CURLcode res;
	std::string data;
	std::string json_file = "response.json";
	
	void writeResponse();
	void parseHTML(std::string& result);
	void saveHTML(std::string& result);
	json createReqJson();
	
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
	void parseJSON();
};
