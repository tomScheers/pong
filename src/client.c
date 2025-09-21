#include "net.h"
#include <netinet/in.h>

int net_client_init_sock() {
  int client_sock = socket(AF_INET, SOCK_STREAM, 0);
  struct sockaddr_in serv_addr;

  serv_addr.sin_family = AF_INET;
  serv_addr.sin_port = htons(PORT);
  serv_addr.sin_addr.s_addr = INADDR_ANY;

  int connect_status =
      connect(client_sock, (struct sockaddr *)&serv_addr, sizeof(serv_addr));
  if (connect_status == -1)
    return -1;

  return client_sock;
}
