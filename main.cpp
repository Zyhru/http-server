#include <csignal>
#include <cstring>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <signal.h>

#include "log/logger.h"
#include "http/response.h"
#include "http/request.h"
#include "openai/openai.h"

class Server {
public:
  int sd;
  sockaddr_in server, client;
  Response rs;
  Request rq;
  void spawn_server(int port);
  void run();
  void grab_ip(char* ip, const struct sockaddr_in* client);
  static void close_connection(int signal);
};

void Server::spawn_server(int port) {

  Logger::Log(Logger::INFO, "Spawning HTTP server with a port of: %d", port);

  server.sin_family = AF_INET;
  server.sin_port = htons(port);
  server.sin_addr.s_addr = INADDR_ANY;

  if((sd = socket(AF_INET, SOCK_STREAM, 0)) == -1) {
    perror("[!] socket()");
    return;
  }

  if((bind(sd, (struct sockaddr *)&server, sizeof(server))) == -1) { 
    perror("[!] bind()");
    return;
  }

  if((listen(sd, 10)) == -1) {
    perror("[!] listen()");
    return;
  }

  Logger::Log(Logger::INFO, "Successfully spawned server");

}

void Server::run() {
  Logger::Log(Logger::INFO, "Running server at localhost:8080. v0.1");
  int cd;
  char ip[INET_ADDRSTRLEN];

  for(;;) {
    socklen_t addr_len = sizeof(client);
    if((cd = accept(sd, (struct sockaddr *)&client, &addr_len)) == -1) {
      perror("[!] socket()");
      return;
    }

    grab_ip(ip, &client);
    Logger::Log(Logger::INFO, "IP address: %s", ip);

    rq.recvRequest(cd);
    rq.sendURL(rs.url_path);
    rs.sendResponse(cd);
    rq.handleRefresh();

    signal(SIGINT, close_connection);
  }

  close(sd);

}

void Server::grab_ip(char* ip, const struct sockaddr_in* client) {
  struct in_addr ip_address = client->sin_addr;
  inet_ntop(AF_INET, &ip_address, ip, INET_ADDRSTRLEN);
}

void Server::close_connection(int signal) {
  Logger::Log(Logger::INFO, "Connection to server has been closed. %d\n", signal);
  exit(0);
}

int main(int i, char *argv[]) {
  OpenAI ai;
  ai.sendCURL();
  
  Server server;
  const int PORT = 8080;
  Logger::Log(Logger::INFO, "Waiting for connections to server.");
  server.spawn_server(PORT);
  server.run();
}

