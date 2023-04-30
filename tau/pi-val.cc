#include "pi-val.hh"
#include "begin.hh"

namespace τ
{


Tt ηo<T> &operator<<(ηo<T> &o, πvc &x)
{
  // Handle the two special cases, which require recursive πv conversion.
  if (x.is_nmap())
  {
    B       b;
    ηoc<B&> c{b};
    {
      ηo<B&> i{c};
      for (let &[k, v] : *x.m()) i.name(k) << v;
    }
    return o << c;
  }

  if (x.is_ntuple())
  {
    B       b;
    ηoc<B&> c{b};
    {
      ηo<B&> i{c};
      for (let &v : *x.v()) i << v;
    }
    return o << c;
  }


  switch (x.t())
  {
  case ηtype::sig:       return o << x.sig();
  case ηtype::n_int:     return o << x.n_int();
  case ηtype::n_float:   return o << x.n_float();
  case ηtype::string:    return o << x.str();
  case ηtype::name:      return o.k(x.name().n);
  case ηtype::atom:      return o << x.atom();
  case ηtype::η:         return o << x.η();
  case ηtype::int8s:     return x.native() ? o << x.vi8s()  : o << x.i8s();
  case ηtype::int16s:    return x.native() ? o << x.vi16s() : o << x.i16s();
  case ηtype::int32s:    return x.native() ? o << x.vi32s() : o << x.i32s();
  case ηtype::int64s:    return x.native() ? o << x.vi64s() : o << x.i64s();
  case ηtype::float32s:  return x.native() ? o << x.vf32s() : o << x.f32s();
  case ηtype::float64s:  return x.native() ? o << x.vf64s() : o << x.f64s();
    TA(o, "ηo<<πv: invalid type " << x.t());
  }
}

Tt ηo<T> &&operator<<(ηo<T> &&o, πvc &x)
{
  return std::move(o << x);
}

template ηo<B&> &operator<<(ηo<B&>&, πvc&);
template ηo<ξ>  &operator<<(ηo<ξ> &, πvc&);

template ηo<B&> &&operator<<(ηo<B&>&&, πvc&);
template ηo<ξ>  &&operator<<(ηo<ξ> &&, πvc&);


sletc cmp_fn = fn
{
  [](auto &&x, auto &&y) -> PO { A(0, "πv<=> internal error"); return PO::unordered; },
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


Tt static O &vout(O &s, T const &xs)
{
  bool first = true;
  for (let &x : xs)
  {
    if (first) first = false;
    else       s << " ";
    s << widen(x);
  }
  return s;
}

Tt static O &mout(O &s, T const &xs)
{
  bool first = true;
  for (let &[k, v] : xs)
  {
    if (first) first = false;
    else       s << ", ";
    s << k << " " << widen(v);
  }
  return s;
}


O &operator<<(O &s, πvc &v)
{
  std::visit(fn {
      [&](i64 x)           { s << x; },
      [&](f64 x)           { s << x; },
      [&](Stvc x)          { s << "\"" << x << "\""; },
      [&](St x)            { s << "\"" << x << "\""; },
      [&](πname x)         { s << "'" << x.n; },
      [&](ηatom x)         { s << x; },
      [&](ηsig x)          { s << x; },
      [&](ηi x)            { s << x; },
      [&](Sp<Re> x)        { s << "[re]"; },
      [&](Sp<V<πv>> x)     { vout(s << "(", *x) << ")"; },
      [&](Sp<M<St, πv>> x) { mout(s << "{", *x) << "}"; },
      [&](Sp<πmf> x)       { s << "[πmf]"; },
      [&](Sp<πdf> x)       { s << "[πdf]"; },
      [&](Sp<πtf> x)       { s << "[πtf]"; },

      [&]<class T, class = If<Isf<T>, void>>(Sp<V<T>> x)
      { vout(s << v.t() << "<", *x) << ">"; },

      [&]<class T>(Sn<T> x)
      { vout(s << v.t() << "<", x) << ">"; }}, v.v_);

  return s;
}


}

#include "end.hh"
