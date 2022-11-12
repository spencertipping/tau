#ifndef τξhttp1_h
#define τξhttp1_h


#include "../strings.hh"


#include "begin.hh"

namespace τ::ξ
{


#if τdebug_iostream
struct http1_state;
O &operator<<(O&, http1_state const&);
#endif


enum class http1_cstate  // what we expect next from the client
{
  http_init,
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
  e_http_body,
  http_done,

  e_ws_res,
  e_ws_init,
  e_ws_cont,
  e_ws_init_pong,
  e_ws_cont_pong,
  ws_done,
};


// TODO: redo this with OOP so we don't have functions scattered all over
// the place.
//
// TODO: decouple client and server states; no need for them to share the
// same struct.

struct http1_state
{
  http1_cstate c;
  http1_sstate s;
  Θp           t0;   // base time for this request
  ΔΘ           rd;   // request-complete deadline (from now)
  B            rb;   // request buffer
  B            rws;  // receiving websocket reassembly buffer
  B            wsk;  // websocket key
  uN           cbl;  // client body length (remaining)
};


void http1_fail(ϝ &f, http1_state &s, chc *m = "HTTP/1.1 400 Bad Request")
{
  if (s.s == http1_sstate::http_init)
  {
    f.α() << m;
    f.α() << "\r\nConnection: close\r\n\r\n";
  }
  s.s = http1_sstate::http_done;
  s.c = http1_cstate::http_done;
  f.α().ω();
  f.β().ω();
}


// TODO: port the below to https://en.cppreference.com/w/cpp/regex

void http1_parse_request(ϝ &f, http1_state &s, uN e)
{
  sletc hws    = cs7(" \t\r\n");  // header whitespace (disallowed)
  slet  http11 = sBv("HTTP/1.1");
  slet  wsconn = sBv("upgrade");
  slet  wsupg  = sBv("websocket");

  // Request data is in the byte range 0..e, where e is the position of
  // the final \r\n\r\n.
  let mu = s.rb.find(' ');                                if (mu == std::string::npos || mu >= e) return http1_fail(f, s);

  uN pl = mu; while (pl < e && s.rb.at(pl) == ' ') ++pl;  if (pl >= e) return http1_fail(f, s);
  let pu = s.rb.find(' ', pl);                            if (pu == std::string::npos || mu >= e || pu <= pl) return http1_fail(f, s);

  uN hl = pu; while (hl < e && s.rb.at(hl) == ' ') ++hl;  if (hl >= e) return http1_fail(f, s);
  let hu = s.rb.find('\r', hl);                           if (hu == std::string::npos || hu <= hl) return http1_fail(f, s);  // impossible

  if (s.rb.substr(hl, hu - hl) != http11
      || s.rb.at(pl) != '/'
      || s.rb.at(hu + 1) != '\n') return http1_fail(f, s);

  M<St, Bv> hs;
  for (uN h0 = hu + 2; h0 < e;)
  {
    if (hws[s.rb.at(h0)]) return http1_fail(f, s, "HTTP/1.1 400 Bad Request -- illegal header whitespace");

    let hnu = s.rb.find(':', h0);
    uN  hvl = hnu + 1; while (hvl < e && s.rb.at(hvl) == ' ') ++hvl;
    let hvu = s.rb.find('\r', hvl);
    if (hvu == std::string::npos) return http1_fail(f, s, "HTTP/1.1 400 Bad Request -- unterminated header value");

    let hn = Bv{s.rb.data() + h0,  hnu - h0};
    let hv = Bv{s.rb.data() + hvl, hvu - hvl};

    for (let c : hn) if (c > 127) return http1_fail(f, s, "HTTP/1.1 400 Bad Request -- illegal header character");

    St hlc; hlc.resize(hn.size());
    std::transform(hn.begin(), hn.end(), hlc.begin(), [](u8 c) { return std::tolower(c); });
    if (hs.contains(hlc)) return http1_fail(f, s, "HTTP/1.1 400 Bad Request -- duplicated header");

    hs[hlc] = hv;
    h0 = hvu + 2;
  }

  let m = Bv{s.rb.data(),  mu};
  let u = Bv{s.rb.data() + pl, pu - pl};
  let h = Bv{s.rb.data() + hl, hu - hl};

  let hscl = hs.contains("content-length");
  let hste = hs.contains("transfer-encoding");
  let hsws = hs.contains("upgrade") && hs.contains("connection")
    && ieq(hs.at("connection"), wsconn)
    && ieq(hs.at("upgrade"),    wsupg);

  if (hsws)
  {
    if (!hs.contains("sec-websocket-key") || !hs.contains("sec-websocket-version")
        || hscl || hste)
      return http1_fail(f, s, "HTTP/1.1 400 Bad Request -- incomplete websocket fields");

    s.wsk = hs.at("sec-websocket-key");
    s.cbl = 0;
    s.c   = http1_cstate::e_ws_init;
    if (!(f.β() << o9t("m", m, "u", u, "p", h, "l", 0, "h", hs, "ws", true).m()))
      return http1_fail(f, s, "HTTP/1.1 500 Internal Server Error");
  }
  else if (hscl && hste)
    return http1_fail(f, s, "HTTP/1.1 400 Bad Request -- transfer-encoding + content-length");
  else if (hscl)
  {
    uN l = 0;
    for (let c : hs.at("content-length"))
      if (cs7("0123456789")[c]) l = l*10 + (c - '0');
      else                      return http1_fail(f, s, "HTTP/1.1 400 Bad Request -- illegal digit in content-length");

    s.cbl = l;
    s.c   = http1_cstate::e_http_body;
    if (!(f.β() << o9t("m", m, "u", u, "p", h, "l", l, "h", hs, "ws", false).m()))
      return http1_fail(f, s, "HTTP/1.1 500 Internal Server Error");
  }
  else if (hste)
  {
    //if (hs.at("transfer-encoding") != sBv("chunked"))
    return http1_fail(f, s, "HTTP/1.1 501 Not Implemented");

    TODO("transfer-encoding: chunked");
    s.cbl = 0;
    s.c   = http1_cstate::e_http_body;
    if (!(f.β() << o9t("m", m, "u", u, "p", h, "l", -1, "h", hs, "ws", false).m()))
      return http1_fail(f, s, "HTTP/1.1 500 Internal Server Error");
  }
  else
  {
    s.cbl = 0;
    s.c   = http1_cstate::e_http_req;
    if (!(f.β() << o9t("m", m, "u", u, "p", h, "l", 0, "h", hs, "ws", false).m()))
      return http1_fail(f, s, "HTTP/1.1 500 Internal Server Error");
  }

  // Important: must happen after all β<< because we alias
  // views against the buffer
  s.rb.erase(0, e + 4);
}


void http1_parser(ϝ &f, http1_state &s)
{
  // TODO: rewrite the logic below; we need to be a lot more careful about
  // how we delineate between "inbound data" and "buffered data"

  for (let x : f.α())
    if (x.real())
    {
      A(x.type() == u9t::bytes,
        "non-bytes " << x.type() << " into http1_parser");

      // Step 1: collect new data into buffer if we're buffering.
      switch (s.c)
      {
      case http1_cstate::http_init:
      case http1_cstate::e_http_req:
        if (s.rb.size() + x.size() > s.rb.capacity())
          return http1_fail(f, s, "HTTP/1.1 431 Request Header Fields Too Large");
        s.rb.append(Sc<Bv>(x));
        break;
      }

    more:
      // Step 2: process whatever buffered data we have.
      switch (s.c)
      {
      case http1_cstate::http_init:
      case http1_cstate::e_http_req:
      {
        // NOTE: avoid n² time complexity by scanning at most three bytes
        // from the prior buffer
        let e = s.rb.find(
          sBv("\r\n\r\n"),
          std::max(0l, Sc<iN>(s.rb.size() - x.size()) - 3));
        if (e != std::string::npos)
        {
          http1_parse_request(f, s, e);
          if (!s.rb.empty()) goto more;
        }
        break;
      }

      case http1_cstate::e_http_body:
      {
        // If any data is in s.rb, process that first; it's leftover
        // buffered data from the request. Then process x if we have
        // anything there. Finally, if x has leftovers, stash those
        // into s.rb.

        if (s.cbl)
        {
          if (!s.rb.empty())
          {
            let n = std::min(s.cbl, s.rb.size());
            s.cbl -= n;
            if (!(f.β() << Bv{s.rb.data(), n})) return http1_fail(f, s, "HTTP/1.1 500 Internal Server Error");
            s.rb.erase(0, n);
            if (!s.rb.empty())  // payload size < buffer size, so the rest is a new request
            {
              s.c = http1_cstate::e_http_req;
              goto more;
            }
          }

          // Regular case: we have inbound byte data, some or all of which
          // is payload.
          {
            let n = std::min(s.cbl, x.size());
            s.cbl -= n;
            if (!(f.β() << Bv{x.data(), n})) return http1_fail(f, s, "HTTP/1.1 500 Internal Server Error");
            if (n < x.size())
            {
              s.c = http1_cstate::e_http_req;
              s.rb = Bv{x.data() + n, x.size() - n};
              goto more;
            }
          }
        }
        else
        {
          s.c = http1_cstate::e_http_req;
        }

        break;
      }

      case http1_cstate::e_ws_init:
      case http1_cstate::e_ws_init_pong:
      {
        // At this point s.rb may contain the beginnings of a websocket
        // frame, so
        break;
      }


      // TODO: decode websocket packets
      // TODO: report connection metrics on ε

      }
    }
    else if (x.τ())
    {
      // Reset state, e.g. for test cases; does not reset timers
      s.c   = http1_cstate::e_http_req;
      s.s   = http1_sstate::http_init;
      s.cbl = 0;
      s.rb.resize(0);
    }
}


void http1_reply(ϝ &f, http1_state &s)
{
  // HTTP replies take a few forms depending on what's being sent:
  //
  // (200, {headers}, b"body")  ← atomic reply
  // (200, {headers}, length)   ← start of streaming reply
  // (200, {headers}, -1)       ← streaming, chunked
  //   b"data"...               ← streaming data
  // ()                         ← end of stream
  //
  // ("ws", 2, b"data")         ← atomic websocket reply (binary)
  // ("ws", 2)                  ← begin streaming
  //   b"data"...               ← data
  // ()                         ← end of data (fin)
  // ("ws", 8)                  ← close
  // ("ws", 9)                  ← ping
  // ("ws", 10)                 ← pong

  for (let x : f.β())
    TODO("http reply");
}


void http1_monitor(ϝ &f, http1_state &s)
{
  f.g.Θ(s.t0 + s.rd);
  if (s.c == http1_cstate::http_init)
    http1_fail(f, s, "HTTP/1.1 408 Request Timeout");
}


// α ↔ TCP-side, β ↔ structured request/response
ϝ &http1(Φ &f,
         uN l  = 8192,  // request size limit
         ΔΘ rt = 5s)    // maximum request time
{
  let s = new http1_state{http1_cstate::http_init,
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
  case http1_cstate::http_init:      return s << "h1/i";
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
