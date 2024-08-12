#include "openai.h"

OpenAI::OpenAI() {
  init();
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
      Logger::Log(Logger::INFO, "found api key..");
  } else {
    Logger::Log(Logger::ERROR, "could not find api key..");
    return;
  }
}

void OpenAI::init() {
  Logger::Log(Logger::INFO, "Initializing libcurl...");
  setAPIKey("OPENAI_API_KEY");

  data = createReqJson().dump();
  chunk.response = (char *)malloc(1);
  chunk.size = 0;
  curl_global_init(CURL_GLOBAL_ALL);
  curl = curl_easy_init();
}

void OpenAI::sendCURL() {
  if(curl) {
    Logger::Log(Logger::INFO, "generating request to openai's API...");
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
      Logger::Log(Logger::ERROR, "unable to send request...");
    } else {
      Logger::Log(Logger::INFO, "successful sending request...");
      //Logger::Log(Logger::INFO, "JSON Payload\n %s", chunk.response);
      std::string result;
      writeResponse();
      parseHTML(result);
      saveHTML(result);
      Logger::Log(Logger::INFO, "Parsed HTML\n%s", result.c_str()); 
    }
  } else {
    Logger::Log(Logger::ERROR, "failed to initialize libcurl.");
  }

}

void OpenAI::writeResponse() {
      Logger::Log(Logger::INFO, "writing JSON payload to response.json.. %s", chunk.response);
      Logger::Log(Logger::INFO, "writing json response to file..");
      std::fstream w_file = u.openFile(json_file, Util::WRITE);
      w_file << chunk.response;
      w_file.close();
      Logger::Log(Logger::INFO, "finished writing..");
}

void OpenAI::parseHTML(std::string& result) {
      std::fstream r_file = u.openFile(json_file, Util::READ);
      json content_res = json::parse(r_file);
      json html = content_res["choices"][0]["message"]["content"];
      std::string html_s = html.dump();
      for(int i = 0; i < html_s.size(); ++i) {
        if(html_s[i] == '\\' && html_s[i+1] == 'n' || 
           html_s[i] == '\\' && html_s[i+1] == '"') {
          ++i;
          continue;
        }
        if(html_s[i] == '"') continue;
        result += html_s[i];
      }
      r_file.close();
}

void OpenAI::saveHTML(std::string& result) {
  std::fstream about_html = u.openFile("html/about.html", Util::WRITE);
  about_html << result;
  about_html.close();
}

json OpenAI::createReqJson() {
  std::stringstream prompt;
  std::fstream prompt_opts = u.openFile("prompt.txt", Util::READ);
  prompt << prompt_opts.rdbuf();

  json request = {
    {"model", "gpt-3.5-turbo"},
    {"messages", json::array({
    {{"role", "user"},  {"content", prompt.str()}}
    })},
  };

 //Logger::Log(Logger::INFO, "Prompt Request: \n%s", request.dump().c_str());
 return request;
}
