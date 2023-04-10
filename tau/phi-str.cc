#include "phi-str.hh"
#include "begin.hh"

namespace τ
{


φr_<St> φcs_::operator()(φc_ const &x) const
{
  uN i = 0;
  while (i < max && i < x.l() && cs[x[i]] ^ n) ++i;
  if (i < min) return x.at(*this).f<St>("too few chars", x.i() + i);
  return x.a(x.sub(i), x.i() + i);
}


φr_<St> φucs_::operator()(φc_ const &x) const
{
  uN n = 0;  // number of parsed chars
  uN i = 0;  // byte offset into input
  while (n < max && i < x.l())
  {
    let c = x[0];
    u64 u = 0;
    uN  w = 0;
    if      ((c & 0x80) == 0)
      w = 1, u = c;
    else if ((c & 0xe0) == 0xc0 && i + 1 < x.l()
             && (x[1] & 0xc0) == 0x80)
      w = 2, u = (c & 0x1f) << 6 | x[1] & 0x3f;
    else if ((c & 0xf0) == 0xe0 && i + 2 < x.l()
             && (x[1] & 0xc0) == 0x80 && (x[2] & 0xc0) == 0x80)
      w = 3, u = (c & 0x0f) << 12 | (x[1] & 0x3f) << 6 | x[2] & 0x3f;
    else if ((c & 0xf8) == 0xf0 && i + 3 < x.l()
             && (x[1] & 0xc0) == 0x80 && (x[2] & 0xc0) == 0x80
             && (x[3] & 0xc0) == 0x80)
      w = 4, u = (c & 0x07) << 18 | (x[1] & 0x3f) << 12 | (x[2] & 0x3f) << 6 | x[3] & 0x3f;
    else break;  // illegal UTF-8 char

    if (!f(u)) { std::cout << "rejecting char " << u << std::endl; break; }
    i += w;
    ++n;
  }
  if (n < min) return x.at(*this).f<St>((Ss{} << "too few chars: " << n).str(), x.i() + i);
  return x.a(x.sub(i), x.i() + i);
}


φr_<V<St>> φre_::operator()(φc_ const &x) const
{
  std::smatch m;
  if (!std::regex_search(x.it(0), x.it(x.l()), m, r))
    return x.at(*this).f<V<St>>("/" + src + "/ failed to match", x.i());

  V<St> y;
  for (let &x : m) y.push_back(x.str());
  return x.a(y, x.i() + m[0].length());
}


}

#include "end.hh"
