#include "pre-container-search.hh"
#include "begin.hh"

namespace σ::pre
{


// Balanced application, so we get log(n) depth
template<class F, class T>
static T bin_apply(F const &f, V<T> &xs)
{
  while (xs.size() > 1)
  {
    V<T> ys;
    ys.reserve(xs.size() / 2 + 1);
    for (uN i = 0; i < xs.size(); i += 2)
      ys.push_back(f(xs[i], xs[i + 1]));
    if (xs.size() & 1) ys.push_back(xs.back());
    xs = ys;
  }
  return xs.back();
}


Sp<iit_> kviat_::query_(ηic &q)
{
  commit();
  let c = q.η().s();
  if (c == "t") return Sp<iit_>(new ηiit(db->get(q.η().next())));
  if (c == "-")
    return Sp<iit_>(new diit(query_(q.η().next()),
                             query_(q.η().next().next())));

  V<Sp<iit_>> xs;
  for (let &x : q.η().next()) xs.push_back(query_(x));
  if      (c == "+") return bin_apply([](Sp<iit_> a, Sp<iit_> b) { return Sp<iit_>(new uiit(a, b)); }, xs);
  else if (c == "*") return bin_apply([](Sp<iit_> a, Sp<iit_> b) { return Sp<iit_>(new iiit(a, b)); }, xs);
  else
  {
    A(0, "unknown query operator: " << c << " in " << q);
    return nullptr;
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
