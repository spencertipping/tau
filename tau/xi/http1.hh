#ifndef τξhttp1_h
#define τξhttp1_h


#include "../strings.hh"


#include "begin.hh"

namespace τ::ξ
{


enum class http1_cstate  // what we expect next from the client
{
  e_http_req,
  e_http_body,
  http_done,

  e_ws_init,
  e_ws_cont,
  e_ws_init_pong,
  e_ws_cont_pong,
  ws_done,
};

enum class http1_sstate  // what the client expects next from us
{
  http_init,
  e_http_res,
  e_http_body,
  http_done,

  e_ws_res,
  e_ws_init,
  e_ws_cont,
  e_ws_init_pong,
  e_ws_cont_pong,
  ws_done,
};


struct http1_state
{
  http1_cstate c;
  http1_sstate s;
  Θp           t0;  // base time for this request
  ΔΘ           rd;  // request-complete deadline (from now)
  B            rb;  // request buffer
};


void http1_fail(ϝ &f, http1_state &s, char const *m)
{
  s.c = http1_cstate::http_done;
  f.α() << m;
  f.α() << "\r\nConnection: close\r\n\r\n";
  f.α().ω();
  f.β().ω();
}


inline void http1_badreq(ϝ &f, http1_state &s)
{
  http1_fail(f, s, "HTTP/1.1 400 Bad Request");
}


void http1_parse_request(ϝ &f, http1_state &s, uN e)
{
  sletc hws = cs7(" \t\r\n");  // header whitespace (disallowed)

  // Request data is in the byte range 0..e, where e is the position of
  // the final \r\n\r\n.
  let mu = s.rb.find(' ');                                if (mu == std::string::npos || mu >= e) return http1_badreq(f, s);

  uN pl = mu; while (pl < e && s.rb.at(pl) == ' ') ++pl;  if (pl >= e) return http1_badreq(f, s);
  let pu = s.rb.find(' ', pl);                            if (pu == std::string::npos || mu >= e || pu <= pl) return http1_badreq(f, s);

  uN hl = pu; while (hl < e && s.rb.at(hl) == ' ') ++hl;  if (hl >= e) return http1_badreq(f, s);
  let hu = s.rb.find('\r', hl);                           if (hu == std::string::npos || hu <= hl) return http1_badreq(f, s);  // impossible

  if (s.rb.substr(hl, hu - hl) != Bv{Rc<u8c*>("HTTP/1.1")}
      || s.rb.at(pl) != '/'
      || s.rb.at(hu + 1) != '\n') return http1_badreq(f, s);

  V<Bv> hs;
  for (uN h0 = hu + 2; h0 < e;)
  {
    if (hws[s.rb.at(h0)]) return http1_badreq(f, s);

    let hnu = s.rb.find(':', h0);
    uN  hvl = hnu + 1; while (hvl < e && s.rb.at(hvl) == ' ') ++hvl;
    let hvu = s.rb.find('\r', hvl);
    if (hvu == std::string::npos) return http1_badreq(f, s);

    hs.push_back(Bv{s.rb.data() + h0,  hnu - h0});
    hs.push_back(Bv{s.rb.data() + hvl, hvu - hvl});
    h0 = hvu + 2;
  }

  f.β() << o9t(s.rb.substr(0, mu),
               s.rb.substr(pl, pu - pl),
               hs);

  // TODO: figure out whether we expect a body by looking for content-length
  // and/or transfer-encoding headers
  s.c = http1_cstate::e_http_body;
  s.rb.erase(0, e + 4);
}


void http1_parser(ϝ &f, http1_state &s)
{
  for (let x : f.α())
    if (x.real())
      switch (s.c)
      {
      case http1_cstate::e_http_req:
        A(x.type() == u9t::bytes,
          "non-bytes " << x.type() << " into http1_parser");
        if (s.rb.size() + x.size() > s.rb.capacity())
          return http1_fail(f, s, "HTTP/1.1 431 Request Header Fields Too Large");
        s.rb.append(Sc<Bv>(x));

        // NOTE: avoid n² time complexity by scanning at most three bytes
        // from the prior buffer
        let e = s.rb.find(
          Bv{Rc<u8c*>("\r\n\r\n")},
          std::max(0l, Sc<iN>(s.rb.size() - x.size()) - 3));
        if (e != std::string::npos) http1_parse_request(f, s, e);
        break;

        // TODO: append to b and parse HTTP request
        // TODO: decode websocket packets
        // TODO: report connection metrics on ε

      }
    else if (x.τ())
    {
      // Reset state, e.g. for test cases; does not reset timers
      s.c = http1_cstate::e_http_req;
      s.s = http1_sstate::http_init;
      s.rb.resize(0);
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
  f.g.Θ(s.t0 + s.rd);
  if (s.c == http1_cstate::e_http_req)
    http1_fail(f, s, "HTTP/1.1 408 Request Timeout");
}


// α ↔ TCP-side, β ↔ structured request/response
ϝ &http1(Φ &f,
         uN l  = 8192,  // request size limit
         ΔΘ rt = 5s)    // maximum request time
{
  let s = new http1_state{http1_cstate::e_http_req,
                          http1_sstate::http_init,
                          now(),
                          rt};
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
  case http1_cstate::e_http_req:     return s << "h1/E[req]";
  case http1_cstate::e_http_body:    return s << "h1/E[b]";
  case http1_cstate::http_done:      return s << "h1∅";

  case http1_cstate::e_ws_init:      return s << "ws/i";
  case http1_cstate::e_ws_cont:      return s << "ws/k";
  case http1_cstate::e_ws_init_pong: return s << "ws/i'";
  case http1_cstate::e_ws_cont_pong: return s << "ws/k'";
  case http1_cstate::ws_done:        return s << "ws∅";
  }
}

O &operator<<(O &s, http1_sstate t)
{
  switch (t)
  {
  case http1_sstate::http_init:      return s << "h1/i";
  case http1_sstate::e_http_res:     return s << "h1/E[res]";
  case http1_sstate::e_http_body:    return s << "h1/E[b]";
  case http1_sstate::http_done:      return s << "h1∅";

  case http1_sstate::e_ws_res:       return s << "ws/E[res]";
  case http1_sstate::e_ws_init:      return s << "ws/i";
  case http1_sstate::e_ws_cont:      return s << "ws/k";
  case http1_sstate::e_ws_init_pong: return s << "ws/i'";
  case http1_sstate::e_ws_cont_pong: return s << "ws/k'";
  case http1_sstate::ws_done:        return s << "ws∅";
  }
}

O &operator<<(O &s, http1_state const &t)
{
  return s << "h1[c=" << t.c << " s=" << t.s << "]";
}
#endif


}

#include "end.hh"


#endif
