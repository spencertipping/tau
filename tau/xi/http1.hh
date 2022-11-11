#ifndef τξhttp1_h
#define τξhttp1_h


#include "begin.hh"

namespace τ::ξ
{


// TODO: split into client-state and server-state
enum class http1_cstate
{
  http_initial,
  http_parsing_request,  // → ws_upgrading if WS, else requested
  http_requested,
  http_sending_reply,    // → initial if keepalive, else done
  http_done,

  ws_upgrading,
  ws_upgraded,
  ws_awaiting_fin,       // expecting a continuation frame
  ws_done,
};


struct http1_state
{
  http1_cstate c;
  Θp           rd;  // request-complete deadline
  B            rb;  // request buffer
};


void http1_parser(ϝ &f, http1_state &s)
{
  for (let x : f.α())
    if (x.real())
    {
      A(x.type() == u9t::bytes,
        "non-bytes " << x.type() << " into http1_parser");
      if (s.rb.size() + x.size() > s.rb.capacity())
      {
        s.c = http1_cstate::http_done;
        f.α() << "HTTP/1.1 413 Payload Too Large\r\nConnection: close\r\n\r\n";
        f.α().ω();
        break;
      }

      s.rb.append(Sc<Bv>(x));

      let e = s.rb.find(Bv{Rc<u8c*>("\r\n\r\n")});
      if (e != std::string::npos)
      {
        f.β() << Bv{s.rb.data(), e};
        // TODO: figure out what type of request this is
      }

      // TODO: append to b and parse HTTP request
      // TODO: decode websocket packets
      // TODO: report connection metrics on ε
    }
}


void http1_reply(ϝ &f, http1_state &s)
{
  for (let x : f.β())
    TODO("http1_reply");
  // TODO: define schema for HTTP replies
  // TODO: how do we send files?
  // TODO: websocket replies
}


void http1_monitor(ϝ &f, http1_state &s)
{
  f.g.Θ(s.rd);
  if (s.c == http1_cstate::http_initial
      || s.c == http1_cstate::http_parsing_request
      || s.c == http1_cstate::ws_upgrading)
  {
    s.c = http1_cstate::http_done;
    f.α() << "HTTP/1.1 408 Request Timeout\r\nConnection: close\r\n\r\n";
    f.α().ω();
  }
}


// α ↔ TCP-side, β ↔ structured request/response
ϝ &http1(Φ &f,
         uN l  = 8192,  // request size limit
         ΔΘ rt = 5s)    // maximum request time
{
  let s = new http1_state{http1_cstate::http_initial, now() + rt};
  s->rb.reserve(l);
  return (new ϝ(f, "http1", ϝ::ξϊ,
                [&, s](ϝ &f) { http1_parser (f, *s); },
                [&, s](ϝ &f) { http1_reply  (f, *s); },
                [&, s](ϝ &f) { http1_monitor(f, *s); }))
    ->xf([s](ϝ &f) { delete s; });
}


#if τdebug_iostream
O &operator<<(O &s, http1_cstate t)
{
  switch (t)
  {
  case http1_cstate::http_initial:         return s << "h1/init";
  case http1_cstate::http_parsing_request: return s << "h1/pr";
  case http1_cstate::http_requested:       return s << "h1/r";
  case http1_cstate::http_sending_reply:   return s << "h1/sr";
  case http1_cstate::http_done:            return s << "h1∅";

  case http1_cstate::ws_upgrading:         return s << "ws/→u";
  case http1_cstate::ws_upgraded:          return s << "ws/u";
  case http1_cstate::ws_awaiting_fin:      return s << "ws/·";
  case http1_cstate::ws_done:              return s << "ws∅";
  }
}
#endif


}

#include "end.hh"


#endif
