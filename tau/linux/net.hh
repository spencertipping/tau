#ifndef τlinux_net_h
#define τlinux_net_h

#include <netdb.h>
#include <netinet/in.h>
#include <sys/types.h>
#include <sys/socket.h>

#include "../Gamma.hh"
#include "../begin.hh"

namespace τ
{


// Static setup ⇒ τe → sock_fd...
Γ tcp_server(u16, u32 = INADDR_LOOPBACK);


// (IP, port) → sock_fd
Γ tcp_client();


Γ http_server();


}

#include "../end.hh"

#endif
