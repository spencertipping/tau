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
  πv  h; h.v_ = Sp<M<St, πv>>{new M<St, πv>};
  for (std::sregex_iterator it(hs.begin(), hs.end(), hr), end_it;
       it != end_it;
       ++it)
    (*h.m())[(*it)[1].str()] = (*it)[2].str();

  w << h;
  return true;
}


static void http_req_loop(ξi i, ξo o)
{
  St b;
  for (let x : i)
  {
    if (x.is_s()) b.append(x.s().begin(), x.s().end());
    if (b.find("\r\n\r\n") != St::npos)
    {
      if (http_req_parse(b, o)) b.clear();
      else                      break;
    }
  }
}


static void http_res_loop(ξi i, ξo o)
{
  for (let x : i)
  {
    // FIXME
    o << x;
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
