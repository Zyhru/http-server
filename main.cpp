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
#include "buffer.h"

class Server {
public:
  Server();
  ~Server();
  Response rs;
  Request rq;
  int sd, cd;
  sockaddr_in server, client;
  void spawn_server(int port);
  void run();
  void grab_ip(char* ip, const struct sockaddr_in* client);
  static void close_connection(int signal);
};

Server::Server() {
  Logger::Log(Logger::INFO, "Waiting for connections to server.");
}

Server::~Server() {
  close(sd);
  close(cd);
}

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
  char ip[INET_ADDRSTRLEN];
  Buffer buffer(4096);

  for(;;) {
    socklen_t addr_len = sizeof(client);
    if((cd = accept(sd, (struct sockaddr *)&client, &addr_len)) == -1) {
      perror("[!] socket()");
      return;
    }

    grab_ip(ip, &client);
    Logger::Log(Logger::INFO, "IP address: %s", ip);

    buffer.recv_data(cd, rq);
    rq.sendURL(rs.url_path);
    buffer.send_data(cd, rs);
    rq.handleRefresh();
    signal(SIGINT, close_connection);
  }

}

void Server::grab_ip(char* ip, const struct sockaddr_in* client) {
  struct in_addr ip_address = client->sin_addr;
  inet_ntop(AF_INET, &ip_address, ip, INET_ADDRSTRLEN);
}

void Server::close_connection(int signal) {
  Logger::Log(Logger::INFO, "Connection to server has been closed. %d\n", signal);
  exit(0);
}

int main(int argc, char *argv[]) {
  const int PORT = 8080;

  OpenAI ai;
  ai.sendCURL();
  Server server;
  server.spawn_server(PORT);
  server.run();
}
