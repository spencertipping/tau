#include "quote.hh"
#include "begin.hh"

namespace σ
{


// NOTE: must have platform-invariant endianness
typedef u64b qN;
sletc qNs = sizeof(qN);


slet quote_ = Ψauto([](ξi i, ξo o)
  {
    // NOTE: each value is prefixed with an 8-byte length header that we can
    // later use to determine when we have the whole thing. We need this because
    // while the _bytes_ of the data are assumed to be intact, each individual
    // string is not preserved.
    for (let x : i)
    {
      qN s = x.lsize();
      o.r(qNs + 1) << Stv{Rc<chc*>(&s), qNs};
      o.r(s + 8)   << Stv{Rc<chc*>(x.odata()), uS(s)};
    }
  });


slet unquote_ = Ψauto([](ξi i, ξo o)
  {
    St b; b.reserve(65536);
    for (let x : i)
    {
      if (x.is_sig())
      {
        if (x.is_τ())
          A(b.empty(), "unquote() received τ sync with non-empty buffer: " << b);
      }
      else if (x.is_s())
      {
        b += x.s();

        // An intact record will be 8 bytes of big-endian length + that many bytes
        // of data. Wait until we have that, then increment.
        uN i = 0;
        for (qN s;
             qNs <= b.size() - i &&
               (s = *Rc<qN*>(b.data() + i)) + qNs <= b.size() - i;
             i += s + qNs)
          o << ηi{Rc<u8c*>(b.data() + i + qNs), uS(s)};
        if (i) b = b.substr(i);
      }
      else
      {
        A(0, "unquote() received invalid η " << x.t() << ": " << x);
      }
    }
  });


Γ quote  (Ψd d) { return ΓΨ(quote_(),   d); }
Γ unquote(Ψd d) { return ΓΨ(unquote_(), d); }


void Γquote(Γφ &g)
{
  g .def_p2(">q", Ψauto(quote_))
    .def_p2("<q", Ψauto(unquote_));
}


}

#include "end.hh"
