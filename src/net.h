#ifndef NETWORK_H
#define NETWORK_H

#include "render.h"
#include <sys/socket.h>

#define PORT 1638

struct DataMsg {
  enum PlayerAction action;
  time_t action_time;
};

int net_serv_init_sock();   // Returns server socket
int net_client_init_sock(); // Returns client socket
int net_serv_conn_client(int listening_sock);
int net_client_conn_serv(
    int client_sock); // Connects to a server, returns server socket
int net_recv_msg(int opp_sock,
                 struct DataMsg *ret_msg); // Passes down the ret_msg directly
                                           // to avoid unnecessary reallocation
int net_send_msg(int opp_sock, struct DataMsg *data);

#endif
