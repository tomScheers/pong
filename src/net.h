#ifndef NETWORK_H
#define NETWORK_H

#include "pong.h"
#include <sys/socket.h>

#define PORT 1638

struct DataMsg {
  enum PlayerAction action;
  time_t action_time;
};

int net_serv_init_sock();                // Returns server socket
int net_client_init_sock();              // Returns client socket
int net_serv_conn_client(int serv_sock); // Waits for a client to connect to
                                         // the server, returns client socket
int net_client_conn_serv(
    int client_sock); // Connects to a server, returns server socket
struct DataMsg *net_recv_msg(int opp_sock);
int net_send_msg(int opp_sock, struct DataMsg *data);

#endif
