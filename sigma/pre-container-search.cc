#include "pre-container-search.hh"
#include "begin.hh"

namespace σ::pre
{


ηsstream kviat_::query_(ηic &q)
{
  commit();
  let c = q.η().s();
  if (c == "t") return ηisstream(db->get(q.η().next()));
  if (c == "-")
    return query_(q.η().next()) - query_(q.η().next().next());

  V<ηsstream> xs;
  for (let &x : q.η().next()) xs.push_back(query_(x));
  if      (c == "+") return ηsstream_union(xs);
  else if (c == "*") return ηsstream_intersect(xs);
  else
  {
    A(0, "unknown query operator: " << c << " in " << q);
    τunreachable();
  }
}


// Query syntax:
//
// ("*" a b ...) -> intersect a b ...
// ("+" a b ...) -> union a b ...
// ("-" a b) -> difference a b
// ("t" x) -> lookup term x
ηm kviat_::query(uN n, ηic &q)
{
  ηm r;
  auto it = query_(q);
  for (uN i = 0; *it && i < n; ++i, ++*it) r << (**it).one().all();
  return r;
}


void kviat_::commit()
{
  if (!s) return;
  ηm x;
  for (let &[k, vs] : xs)
  {
    x.clear();
    V<ηi> av(vs.begin(), vs.end());
    let   bv = db->get(k);
    std::sort(av.begin(), av.end());

    uN t = bv.lsize();
    for (let &v : av) t += v.osize();
    x.reserve(t);

    auto ai = av.begin();
    auto bi = bv.begin();
    let  ae = av.end();
    let  be = bv.end();

    while (ai != ae && bi != be)
    {
      let a = (*ai).one();
      let b = (*bi).one();
      let c = a <=> b;
      if      (c == PO::less)       x << a.all(), ++ai;
      else if (c == PO::greater)    x << b.all(), ++bi;
      else if (c == PO::equivalent) x << a.all(), ++ai, ++bi;
      else A(0, a << " <=> " << b << " is undefined");
    }
    while (ai != ae) x << (*ai++).one().all();
    while (bi != be) x << (*bi).one().all(), ++bi;

    db->set(k, x);
  }

  xs.clear();
  s = 0;
}


Sp<at_> iat(cback const &b, Sp<kv_> kv)
{
  return Sp<at_>(new kviat_(b, kv));
}


}

#include "end.hh"
