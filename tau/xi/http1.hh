#ifndef τξhttp1_h
#define τξhttp1_h


#include "begin.hh"

namespace τ::ξ
{


void http1_parser(ϝ &f, uN l)
{
  B b;
  b.reserve(l);

  for (let x : f.α())
    if (x.exists())
    {
      // TODO: append to b and parse HTTP request
      // TODO: decode websocket packets
      // TODO: report connection metrics on ε
    }
}


void http1_reply(ϝ &f)
{
  // TODO: define schema for HTTP replies
  // TODO: how do we send files?
  // TODO: websocket replies
}


void http1_monitor(ϝ &f)
{
  // TODO: on kill switch, disconnect φs
}


ϝ &http1(Φ &f, uN l = 8192)  // l = upper size limit on HTTP requests
{
  // TODO: shared data so we can encode connection state

  return *new ϝ(f, "http1", ϝ::ξϊ,
                [&, l](ϝ &f) { http1_parser(f, l); },
                http1_reply,
                http1_monitor);
}


}

#include "end.hh"


#endif
