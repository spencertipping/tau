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
  Re  r(R"(^([A-Z]+) ([^ ]+) HTTP/1\.1\r?\n((?:[A-Za-z0-9-]+: [^\r\n]*\r?\n)+)\r?\n)");
  Rsm m;

  if (!std::regex_match(b, m, r)) return false;

  auto w = o.r(b.size() + 256);
  w << m[1].str() << m[2].str();

  Stc hs = m[3].str();
  Re  hr(R"(([A-Za-z0-9-]+):[ \t]*([^\r\n]*)\r?\n)");
  for (Rsi i(hs.begin(), hs.end(), hr), e; i != e; ++i)
    w << ηname{(*i)[1].str()} << (*i)[2].str();

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
    else if (x.is_s()) o << x;
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
