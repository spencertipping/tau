#include "pi-val.hh"
#include "begin.hh"

namespace τ
{


sletc cmp_fn = fn
{
  [](auto &&x, auto &&y) -> PO { throw "πvc<=> internal error"; },
  []<class T, class U,
     class = decltype(std::declval<T>().size()),
     class = decltype(std::declval<U>().size()),
     class = decltype(std::declval<T>()[0] <=> std::declval<U>()[0])>
    (T const &x, U const &y) { return svc(x, y); }
};


PO πv::operator<=>(πvc &x) const
{
  let ta = t();
  let tb = x.t();
  if (ta == ηtype::invalid || tb == ηtype::invalid)
    return PO::unordered;

  let tc = ta <=> tb;
  if (tc != SO::equal) return tc;

  switch (ta)
  {
  case ηtype::sig:     return sig()     <=> x.sig();
  case ηtype::n_int:   return n_int()   <=> x.n_int();
  case ηtype::n_float: return n_float() <=> x.n_float();
  case ηtype::string:  return str()     <=> x.str();
  case ηtype::atom:    return atom()    <=> x.atom();
  case ηtype::name:    return name().n  <=> x.name().n;

  case ηtype::int8s:
  case ηtype::int16s:
  case ηtype::int32s:
  case ηtype::int64s:
  case ηtype::float32s:
  case ηtype::float64s: return std::visit(cmp_fn, v_, x.v_);

  case ηtype::η: return ηscmp(η(), x.η());

  default: return PO::unordered;
  }
}


O &operator<<(O &s, πvc &v)
{
  std::visit(fn {
      [&](Sp<Re> x) { s << "[regex]"; },
      [&](Sp<V<πv>> x) { s << "(...)"; },
      [&](Sp<M<St, πv>> x) { s << "{...}"; },
      [&](i64 x) { s << x; },
      [&](f64 x) { s << x; },
      [&](St x) { s << "\"" << x << "\""; },
      [&](πname x) { s << "'" << x.n; },
      [&](ηatom x) { s << x; },
      [&](ηsig x) { s << x; },
      [&](ηi x) { s << x; },
      [&]<class T>(Sp<V<T>> x)
      { s << v.t() << "<";
        bool first = true;
        for (let y : *x)
        { if (first) first = false;
          else       s << " ";
          s << widen(y); }
        s << ">"; },
      [&]<class T>(Sn<T> x)
      { s << v.t() << "<";
        bool first = true;
        for (let y : x)
        { if (first) first = false;
          else s << " ";
          s << widen(y); }
        s << ">"; }}, v.v_);
  return s;
}


}

#include "end.hh"
