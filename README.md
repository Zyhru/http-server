# About
Basic HTTP server is written in C++ that generates html files straight from ChatGPT.

## How To Run
Running the server is easy. At the moment it will spawn a server at port 8080. 

```c++
git clone <remote-url>
Make sure to set the environment variable OPENAI_API_KEY to interact with the API.
./build.sh
```
## Features being thought of/added
+ CLI to read a prompt and send it as a request.
+ CLI to spawn server at different ports.
