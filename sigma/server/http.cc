#include "http.hh"
#include "../begin.hh"

namespace σ
{


void Γhttp(Γφ &g)
{
  g.def_c4("H", [](ψ q, ξi fi, ξo fo, ξo bo, ξi bi)
    {
      q.f([=]() { http_req_loop(fi, fo); });
      q.f([=]() { http_res_loop(bi, bo); });
    });
}


bool http_req_parse(Stc &b, ξo o)
{
  // NOTE: std::regex is unsafe here because it segfaults (stack overflows)
  // on large values. We have to parse the request manually.
  let  me  = b.find(' ');
  let  ue  = b.find(' ', me + 1);
  auto l1e = b.find('\n', ue + 1);
  if (l1e == St::npos) return false;

  auto w = o.r(b.size() + 256);
  w << b.substr(0, me) << b.substr(me + 1, ue - (me + 1));

  for (auto le = l1e, hls = l1e + 1;  // header line start
       le != St::npos;
       hls = le + 1)
  {
    le = b.find('\n', hls);
    if (le == St::npos) return false;  // bogus HTTP request
    let hve = b[le - 1] == '\r' ? le - 1 : le;

    if (hve == hls) break;  // empty line (end of headers)

    let hc = b.find(':', hls);
    auto hvs = hc + 1;
    while (hvs < hve && (b[hvs] == ' ' || b[hvs] == '\t')) ++hvs;
    w << ηname{b.substr(hls, hc - hls)} << b.substr(hvs, hve - hvs);
  }

  return true;
}


// Input: chunks of HTTP request data, from the socket
// Output: η[method url headers...]
void http_req_loop(ξi i, ξo o)
{
  // TODO: make this a proper state machine with a buffer, content-length,
  // and transfer-encoding
  St b;
  for (let &x : i)
    if (x.is_s() || x.is_bin())
    {
      b.append(x.data(), x.data() + x.size());
      if (b.find("\r\n\r\n") != St::npos || b.find("\n\n") != St::npos)
      {
        if (http_req_parse(b, o)) b.clear();
        else
          A(0, "http_req_loop: parse error on " << b);
      }
    }
}


// Input: η[code message headers...] body... τ
// Output: chunks of HTTP response data, to the socket
void http_res_loop(ξi i, ξo o)
{
  for (let &x : i)
    if (x.is_i() && x.has_next())  // structured reply
    {
      let [code, msg] = ηT<i64, St>(x);
      Ss r; r << "HTTP/1.1 " << code << " " << msg << "\r\n";
      ηi h = x.next().next();
      while (h.has_next())
      {
        r << h.cs() << ": " << h.next().cs() << "\r\n";
        h = h.next();
        if (h.has_next()) h = h.next();
        else              break;
      }
      r << "\r\n";
      let s = r.str();
      o.r(s.size() + 8) << s;
    }
    else if (x.is_s() || x.is_bin()) o << x;
    else if (x.is_τ())  // end of reply
    {
      // TODO: reset state
    }
    else
    {
      A(0, "http_res_loop: unexpected input " << x);
    }
}


}

#include "../end.hh"
