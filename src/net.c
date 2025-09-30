#include "net.h"

#include <errno.h>
#include <stdio.h>
#include <sys/socket.h>
#include <sys/time.h>

int net_recv_msg(int opp_sock, struct DataMsg *ret_msg) {
  int n = recv(opp_sock, ret_msg, sizeof(*ret_msg), 0);
  if (n < 0) {
    if (errno == EWOULDBLOCK || errno == EAGAIN) {
      return 0;
    } else {
      perror("recv");
      return -1;
    }
  }
  return n;
}

int net_send_msg(int opp_sock, struct DataMsg *data) {
  return send(opp_sock, data, sizeof(*data), 0);
}
