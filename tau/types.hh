#ifndef τtypes_h
#define τtypes_h


#include <chrono>
#include <memory>
#include <type_traits>


#include "arch.hh"
#include "debug.hh"

#include "ctypes.hh"
#include "cptr.hh"

#include "begin.hh"


#define τuse_nonvolatile_sharedptr 1


namespace τ
{

using namespace std::literals;
using namespace std::placeholders;


#if !τuse_nonvolatile_sharedptr
  template<class... T> using Sp = std::shared_ptr<T...>;
  template<class... T> using Wp = std::weak_ptr<T...>;

  template<class U, class T> Sp<U> dpc(Sp<T> const &x) { return std::dynamic_pointer_cast<U>(x); }
  template<class T>          T*    wpg(Wp<T> const &x) { return x.lock().get(); }
  template<class T>          Sp<T> ms (T &&x)          { return std::make_shared(mo(x)); }
#else
  template<class... T> using Sp = shared_ptr<T...>;
  template<class... T> using Wp = weak_ptr<T...>;

  template<class U, class T> Sp<U> dpc(Sp<T> const &x) { return x.template as<U>(); }
  template<class T>          T*    wpg(Wp<T> const &x) { return x.get(); }
  template<class T>          Sp<T> ms (T &&x)          { let r = Sp<T>(new T{}); *r = mo(x); return r; }
#endif


// Span/vector comparisons
template<class T, class U>
PO svc(T const &a, U const &b)
{
  let sl = std::min(a.size(), b.size());
  for (uN i = 0; i < sl; ++i)
  {
    let c = a[i] <=> b[i];
    if (c != SO::equal) return c;
  }
  if (a.size() > sl) return PO::greater;
  if (b.size() > sl) return PO::less;
  return PO::equivalent;
}

template<class T, class U,
         class = decltype(std::declval<T>() <=> std::declval<U>())>
PO operator<=>(Sn<T> const &a, Sn<U> const &b) { return svc(a, b); }

template<class T, class U,
         class = decltype(std::declval<T>() <=> std::declval<U>())>
PO operator<=>(V<T> const &a, Sn<U> const &b) { return svc(a, b); }

template<class T, class U,
         class = decltype(std::declval<T>() <=> std::declval<U>())>
PO operator<=>(Sn<T> const &a, V<U> const &b) { return svc(a, b); }

template<class T, class U,
         class = decltype(std::declval<T>() <=> std::declval<U>())>
PO operator<=>(V<T> const &a, V<U> const &b) { return svc(a, b); }


#if τclang
// clang doesn't define this for string views, so we need to create
// our own. Immediately shell out to <=> for spans, comparing chars.
inline PO operator<=>(Stvc &a, Stvc &b)
{
  return Sn<chc>(a.data(), a.size()) <=> Sn<chc>(b.data(), b.size());
}
#endif


// std::variant superset casting
template<class... Xs>
struct vc_
{
  Va<Xs...> v;

  template<class... Ys>
  operator Va<Ys...>() &&
    { return std::visit([](auto &&x) -> Va<Ys...> { return x; }, mo(v)); }
};

template<class... Xs> vc_<Xs...> vc(Va<Xs...> &&v) { return {mo(v)}; }
template<class... Xs> vc_<Xs...> vc(Va<Xs...> &v)  { return {v}; }


// Polymorphic functions
template<class... Xs> struct fn : Xs... { using Xs::operator()...; };
template<class... Xs> fn(Xs...) -> fn<Xs...>;


typedef std::chrono::steady_clock       Θc;
typedef std::chrono::nanoseconds        ΔΘ;
typedef std::chrono::time_point<Θc, ΔΘ> Θp;

// Definition of time quantum
ΔΘ constexpr Θq = 1ns;
static_assert(Θq.count() == 1);
static_assert(std::is_integral<ΔΘ::rep>::value);


// Construct next available key for map
template<class K, class V>
inline K ιi(K &c, M<K, V> const &m)
{
  while (m.contains(++c) || !c);
  return c;
}


struct ι  // ranged numeric iteration
{
  uNc l;
  uNc h;

  ι(uN h_)        : l(0),  h(h_) {}
  ι(uN l_, uN h_) : l(l_), h(h_) {}

  struct it
  {
    uN x;
    uN   operator* ()            const { return x; }
    it  &operator++()                  { ++x; return *this; }
    bool operator==(it const &y) const { return x == y.x; }
  };

  it begin() const { return it{l}; }
  it end  () const { return it{h}; }
};


}

#include "end.hh"


#endif
