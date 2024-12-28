#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/stat.h>

#define PORT 3000
#define BUFFER_SIZE 1024
#define HTML_DIR "./html"

void get_file_path(char *path, char *full_path) {
  if (strcmp(path, "/") == 0) {
    snprintf(full_path, BUFFER_SIZE, "%s/index.html", HTML_DIR);
  } else {
    snprintf(full_path, BUFFER_SIZE, "%s%s.html", HTML_DIR, path);
  }
}

void send_file(int client_socket, const char *file_path) {
  FILE *file = fopen(file_path, "r");
  if (!file) {
    const char *custom_404_path = "./html/404.html";
    FILE *custom_404 = fopen(custom_404_path, "r");
    if (custom_404) {
      const char *response_header =
        "HTTP/1.1 404 Not Found\r\n"
        "Content-Type: text/html\r\n"
        "Connection: close\r\n\r\n";
      send(client_socket, response_header, strlen(response_header), 0);

      char file_buffer[BUFFER_SIZE];
      size_t bytes_read;
      while ((bytes_read = fread(file_buffer, 1, sizeof(file_buffer), custom_404)) > 0) {
        send(client_socket, file_buffer, bytes_read, 0);
      }
      
      fclose(custom_404);
    } else {
      const char *not_found_response =
        "HTTP/1.1 404 Not Found\r\n"
        "Content-Type: text/plain\r\n"
        "Connection: close\r\n\r\n"
        "404 Not Found";
      send(client_socket, not_found_response, strlen(not_found_response), 0);
    }

    shutdown(client_socket, SHUT_WR);
    close(client_socket);
    return;
  }

  const char *response_header =
    "HTTP/1.1 200 OK\r\n"
    "Content-Type: text/html\r\n"
    "Connection: close\r\n\r\n";
  send(client_socket, response_header, strlen(response_header), 0);

  char file_buffer[BUFFER_SIZE];
  while (fgets(file_buffer, sizeof(file_buffer), file)) {
    send(client_socket, file_buffer, strlen(file_buffer), 0);
  }

  fclose(file);
  shutdown(client_socket, SHUT_WR);
  close(client_socket);
}

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

  if (strcmp(method, "GET") != 0) {
    const char *not_allowed_response =
      "HTTP/1.1 405 Method Not Allowed\r\n"
      "Content-Type: text/plain\r\n"
      "Connection: close\r\n\r\n"
      "405 Method Not Allowed";
    send(client_socket, not_allowed_response, strlen(not_allowed_response), 0);
    close(client_socket);
    return;
  }

  char full_path[BUFFER_SIZE];
  get_file_path(path, full_path);
  send_file(client_socket, full_path);
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
