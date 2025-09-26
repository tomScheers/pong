#include "net.h"

#include <netinet/in.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/socket.h>
#include <sys/types.h>

int net_serv_init_sock() {
  int serv_sock = socket(AF_INET, SOCK_STREAM, 0);

  struct sockaddr_in serv_addr;
  serv_addr.sin_family = AF_INET; // Sets IP to ipv4
  serv_addr.sin_port = htons(PORT);
  serv_addr.sin_addr.s_addr = INADDR_ANY;

  // Binds socket to the IP and port
  if (bind(serv_sock, (struct sockaddr *)&serv_addr, sizeof(serv_addr)) == -1) {
    perror("bind");
    return -1;
  }

  return serv_sock;
}

int net_serv_conn_client(int listening_sock) {
  listen(listening_sock, 1);
  printf("Establishing connection\n");
  int new_sock = accept(listening_sock, NULL, NULL);
  printf("Connection established\n");
  return new_sock;
}
