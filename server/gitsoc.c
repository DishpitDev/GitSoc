#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>

#define PORT 3000
#define BUFFER_SIZE 1024

void handle_client(int client_socket) {
  char buffer[BUFFER_SIZE];
  int bytes_received = recv(client_socket, buffer, sizeof(buffer) - 1, 0);

  if (bytes_received < 0) {
    perror("Error reading from client");
    close(client_socket);
    return;
  }

  buffer[bytes_received] = '\0';
  printf("Received Request: \n%s\n", buffer);

  char method[8], path[256], version[16];
  sscanf(buffer, "%s %s %s", method, path, version);
  printf("HTTP Method: %s\nPath: %s\nHTTP Version: %s\n", method, path, version);

  const char *response =
    "HTTP/1.1 200 OK\r\n"
    "Content-Type: text/plain\r\n"
    "Connection: close\r\n\r\n"
    "Hello, World!";
  send(client_socket, response, strlen(response), 0);

  close(client_socket);
}

int main() {
  int server_socket, client_socket;
  struct sockaddr_in server_addr, client_addr;
  socklen_t client_addr_len = sizeof(client_addr);

  server_socket = socket(AF_INET, SOCK_STREAM, 0);
  if (server_socket < 0) {
    perror("Socket creation failed");
    exit(EXIT_FAILURE);
  }

  server_addr.sin_family = AF_INET;
  server_addr.sin_addr.s_addr = INADDR_ANY;
  server_addr.sin_port = htons(PORT);

  if (bind(server_socket, (struct sockaddr *)&server_addr, sizeof(server_addr)) < 0) {
    perror("Bind failed");
    close(server_socket);
    exit(EXIT_FAILURE);
  }

  if(listen(server_socket, 5) < 0) {
    perror("Listen failed");
    close(server_socket);
    exit(EXIT_FAILURE);
  }

  printf("Server is listening on port %d...\n", PORT);

  while (1) {
    client_socket = accept(server_socket, (struct sockaddr *)&client_addr, &client_addr_len);
    if (client_socket < 0) {
      perror("Accept failed");
      continue;
    }

    printf("New connection established\n");
    handle_client(client_socket);
  }

  close(server_socket);
  return 0;
}
