#include "net.h"

#include <netinet/in.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/socket.h>
#include <sys/types.h>

int net_serv_init_sock() {
  int serv_sock = socket(AF_INET, SOCK_STREAM, 0);

  struct sockaddr_in *serv_addr = malloc(sizeof(*serv_addr));
  if (!serv_addr)
    return -1;
  serv_addr->sin_family = AF_INET; // Sets IP to ipv4
  serv_addr->sin_port = htons(PORT);
  serv_addr->sin_addr.s_addr = INADDR_ANY;

  // Binds socket to the IP and port
  if (bind(serv_sock, (struct sockaddr *)serv_addr, sizeof(*serv_addr)) == -1) {
    perror("bind");
    return -1;
  }

  return serv_sock;
}

int net_serv_conn_client(int serv_sock) {
  listen(serv_sock, 1);
  int client_sock = accept(serv_sock, NULL, NULL);
  return client_sock;
}
