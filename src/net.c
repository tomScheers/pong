#include "net.h"
#include <stdlib.h>

struct DataMsg *net_recv_msg(int opp_sock) {
  struct DataMsg *data = malloc(sizeof(*data));
  if (!data)
    return NULL;

  recv(opp_sock, data, sizeof(*data), 0);

  return data;
}
int net_send_msg(int opp_sock, struct DataMsg *data) {
  send(opp_sock, data, sizeof(*data), 0);
  return 0;
}
