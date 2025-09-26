#include "net.h"

int net_recv_msg(int opp_sock, struct DataMsg *ret_msg) {
  return recv(opp_sock, ret_msg, sizeof(*ret_msg), 0);
}

int net_send_msg(int opp_sock, struct DataMsg *data) {
  return send(opp_sock, data, sizeof(*data), 0);
}
