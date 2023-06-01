#include "psi4.hh"
#include "begin.hh"

namespace σ
{

using namespace τ;


static bool http_req_parse(Stc &b, ξo o)
{
  Re  r(R"(^([A-Z]+) ([^ ]+) HTTP/1\.1\r\n((?:[A-Za-z0-9-]+: [^\r\n]*\r\n)+)\r\n)");
  Rsm m;

  if (!std::regex_match(b, m, r)) return false;

  auto w = o.r(b.size() + 256);
  w << m[1].str() << m[2].str();

  Stc hs = m[3].str();
  Re  hr(R"(([A-Za-z0-9-]+): ([^\r\n]*)\r\n)");
  ηm  h;
  for (Rsi i(hs.begin(), hs.end(), hr), e; i != e; ++i)
    h.k((*i)[1].str()) << (*i)[2].str();

  w << h;
  return true;
}


// Input: chunks of HTTP request data, from the socket
// Output: η[method url {headers}]

static void http_req_loop(ξi i, ξo o)
{
  // TODO: make this a proper state machine with a buffer, content-length,
  // and transfer-encoding
  St b;
  for (let x : i)
  {
    if (x.is_s())
    {
      b.append(x.s().begin(), x.s().end());
      if (b.find("\r\n\r\n") != St::npos)
      {
        if (http_req_parse(b, o)) b.clear();
        else                      break;
      }
    }
  }
}


// Input: η[code message {headers}] body... τ
// Output: chunks of HTTP response data, to the socket

static void http_res_loop(ξi i, ξo o)
{
  for (auto x : i)
  {
    if (x.is_η()) x = x.η();       // TODO: remove this once π is fixed
    if (x.is_i() && x.has_next())  // structured reply
    {
      Ss r; r << "HTTP/1.1 " << x.i() << " " << x.next().s() << "\r\n";
      let h = x.next().next().η();
      for (let i : h)
        if      (i.is_n()) r << i.n() << ": ";
        else if (i.is_s()) r << i.s() << "\r\n";
        else               A(0, "http_res_loop: unexpected header " << i);
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


Ψ4 Γhttp()
{
  return {[](ψ q, ξi fi, ξo fo, ξo bo, ξi bi, Ψaux)
    {
      q.f([=]() { http_req_loop(fi, fo); });
      q.f([=]() { http_res_loop(bi, bo); });
    }, "H", true};
}


}

#include "end.hh"
