#include "openai.h"

OpenAI::OpenAI(const char *env) {
  init();
  setAPIKey(env);
}

OpenAI::~OpenAI() {
  curl_easy_cleanup(curl);
  curl_slist_free_all(headers);
  free(chunk.response);
  curl_global_cleanup();
}

void OpenAI::setAPIKey(const char* env) {
  const char *env_p;
  if((env_p = std::getenv(env))) {
      this->api_key = env_p;
      Logger::Log(Logger::INFO, "Found API Key %s", this->api_key);
  } else {
    // throw exception
  }
}

void OpenAI::init() {
  Logger::Log(Logger::INFO, "Initializing libcurl...");

  auto req = R"({
    "model" : "gpt-3.5-turbo",
    "messages": [
        {
          "role": "user",
          "content": "generate me a minilistic html file" 
        }
     ]
    }
  )"_json;
  data = req.dump();
  chunk.response = (char *)malloc(1);
  chunk.size = 0;
  curl_global_init(CURL_GLOBAL_ALL);
  curl = curl_easy_init();
}

void OpenAI::sendCURL() {
  if(curl) {
    std::string apiKey = "Authorization: Bearer ";
    apiKey += api_key;
    headers = curl_slist_append(headers, "Content-Type: application/json");
    headers = curl_slist_append(headers, apiKey.c_str());
    curl_easy_setopt(curl, CURLOPT_URL, "https://api.openai.com/v1/chat/completions");
    curl_easy_setopt(curl, CURLOPT_HTTPHEADER, headers);
    curl_easy_setopt(curl, CURLOPT_POSTFIELDS, data.c_str());
    curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, write_data);
    curl_easy_setopt(curl, CURLOPT_WRITEDATA, (void *)&chunk);
    curl_easy_setopt(curl, CURLOPT_CUSTOMREQUEST, "POST");

    res = curl_easy_perform(curl);
    if(res != CURLE_OK) {
      Logger::Log(Logger::ERROR, "unable to receive response...");
    } else {
      // TODO: Parse response by grabbing the content of the message and writing to a file
      json res = chunk.response;
      Logger::Log(Logger::INFO, "OpenAI Response: \n%s", res.dump().c_str());
    }
  } else {
    // throw exception
    Logger::Log(Logger::ERROR, "failed to initialize libcurl.");
  }
}
