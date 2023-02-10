#ifndef τlinux_net_h
#define τlinux_net_h

#include <netdb.h>
#include <netinet/in.h>
#include <sys/types.h>
#include <sys/socket.h>

#include "../gamma.hh"
#include "../begin.hh"

namespace τ
{


// Static setup ⇒ τe → sock_fd...
Sp<γ_> tcp_server(u16, u32 = INADDR_LOOPBACK);


// (IP, port) → sock_fd
Sp<γ_> tcp_client();


}

#include "../end.hh"

#endif
