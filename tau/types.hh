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


template<class T> using Id = typename std::type_identity<T>::type;


// Return a null (exploding) reference to a type
template<class T> T &null() { return *(T*)nullptr; }


// Widening away from i8/u8 so we can print stuff easily
template<class T> If<sizeof(T) == 1, int> widen(T x) { return x; }
template<class T> If<sizeof(T) != 1, T>   widen(T x) { return x; }


// Tuple slicing: Tsub<1, 3, T<char, short, int, long>> = T<short, int, long>
template<class... Xs, uS... I>
auto tat_(T<Xs...> const &t, Is<I...>) { return std::make_tuple(std::get<I>(t)...); }

template<class X, class Y>
auto tcons(X &&x, Y &&xs) { return std::tuple_cat(std::make_tuple(std::forward<X>(x)), std::forward<Y>(xs)); }

template<class X, class Y>         struct Tcons_;
template<class... Xs, class... Ys> struct Tcons_<T<Xs...>, T<Ys...>> { using t = T<Xs..., Ys...>; };


template<class X, class... Xs, uS... Is>
T<Xs...> tdrop_(T<X, Xs...> const &xs, std::index_sequence<Is...>)
{ return std::make_tuple(std::get<Is + 1>(xs)...); }

template<class X, class... Xs>
T<Xs...> tdrop(T<X, Xs...> const &xs)
{ return tdrop_(xs, std::index_sequence_for<Xs...>{}); }


template<uS n, class X>  struct offset_;
template<uS n, uS... Xs> struct offset_<n, Is<Xs...>> { using t = Is<Xs + n...>; };
template<uS n, class X>  using  offset = typename offset_<n, X>::t;

template<uS s, uS n, class    X>  struct Tsub_;
template<uS s, uS n, class... Xs> struct Tsub_<s, n, T<Xs...>>
{ using t = decltype(
    tat_(std::declval<T<Xs...>>(),
         std::declval<offset<s, std::make_index_sequence<n>>>())); };

template<uS s, uS n, class X> using Tsub  = typename Tsub_<s, n, X>::t;
template<uS n, class X>       using Ttake = typename Tsub_<0, n, X>::t;
template<uS n, class X>       using Tdrop = typename Tsub_<n, std::tuple_size<X>::value - n, X>::t;


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
