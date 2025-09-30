#include "net.h"

#include <netinet/in.h>
#include <stdio.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <unistd.h>

int net_serv_init_sock() {
  int serv_sock = socket(AF_INET, SOCK_STREAM, 0);
  if (serv_sock == -1) {
    perror("socket");
    return -1;
  }

  struct sockaddr_in serv_addr;
  serv_addr.sin_family = AF_INET; // Sets IP to ipv4
  serv_addr.sin_port = htons(PORT);
  serv_addr.sin_addr.s_addr = INADDR_ANY;

  if (bind(serv_sock, (struct sockaddr *)&serv_addr, sizeof(serv_addr)) == -1) {
    perror("bind");
    close(serv_sock);
    return -1;
  }

  return serv_sock;
}

int net_serv_conn_client(int listening_sock) {
  if (listen(listening_sock, 1) == -1)
    return -1;

  int new_sock = accept(listening_sock, NULL, NULL);
  return new_sock;
}
