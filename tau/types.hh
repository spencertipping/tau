#ifndef τtypes_h
#define τtypes_h


#include <chrono>
#include <memory>
#include <type_traits>


#include "arch.hh"
#include "debug.hh"

#include "ctypes.hh"
#include "cptr.hh"
#include "mptr.hh"

#include "begin.hh"


#define τuse_nonvolatile_sharedptr 1


namespace τ
{

using namespace std::literals;
using namespace std::placeholders;


typedef Ar<u8, 28> h224;  typedef h224 const h224c;
typedef Ar<u8, 32> h256;  typedef h256 const h256c;
typedef Ar<u8, 48> h384;  typedef h384 const h384c;
typedef Ar<u8, 64> h512;  typedef h512 const h512c;

St h_hex(h224c&);
St h_hex(h256c&);
St h_hex(h384c&);
St h_hex(h512c&);

St h_b64(h224c&);
St h_b64(h256c&);
St h_b64(h384c&);
St h_b64(h512c&);

h224 hex_h224(Stc&);
h256 hex_h256(Stc&);
h384 hex_h384(Stc&);
h512 hex_h512(Stc&);

h224 b64_h224(Stc&);
h256 b64_h256(Stc&);
h384 b64_h384(Stc&);
h512 b64_h512(Stc&);

h224 operator^(h224c&, h224c&);
h256 operator^(h256c&, h256c&);
h384 operator^(h384c&, h384c&);
h512 operator^(h512c&, h512c&);


struct bytes { u64 n; };

enum class byte_suffix : u64
{
  one  = 1ull,
  kilo = 1000ull,
  kibi = 1024ull,
  mega = 1000ull * 1000,
  mebi = 1024ull * 1024,
  giga = 1000ull * 1000 * 1000,
  gibi = 1024ull * 1024 * 1024,
  tera = 1000ull * 1000 * 1000 * 1000,
  tebi = 1024ull * 1024 * 1024 * 1024,
  peta = 1000ull * 1000 * 1000 * 1000 * 1000,
  pebi = 1024ull * 1024 * 1024 * 1024 * 1024,
  exa  = 1000ull * 1000 * 1000 * 1000 * 1000 * 1000,
  exbi = 1024ull * 1024 * 1024 * 1024 * 1024 * 1024,
};


O &operator<<(O&, byte_suffix);
O &operator<<(O&, bytes);


#if !τuse_nonvolatile_sharedptr
  Txs using Sp = std::shared_ptr<Xs...>;
  Txs using Wp = std::weak_ptr<Xs...>;

  template<class U, class T> Sp<U> dpc(Sp<T> const &x) { return std::dynamic_pointer_cast<U>(x); }
  Tt          T*    wpg(Wp<T> const &x) { return x.lock().get(); }
  Tt          Sp<T> ms (T &&x)          { return std::make_shared(mo(x)); }
#else
  template<class... T> using Sp = shared_ptr<T...>;
  template<class... T> using Wp = weak_ptr<T...>;

  template<class U, class T> Sp<U> dpc(Sp<T> const &x) { return x.template as<U>(); }
  Tt          T*    wpg(Wp<T> const &x) { return x.get(); }
  Tt          Sp<T> ms (T &&x)          { let r = Sp<T>(new T{}); *r = mo(x); return r; }
#endif

Txs using Mp = mptr<Xs...>;


Tt using Id = typename std::type_identity<T>::type;

Tt  struct is_tuple_           : std::false_type {};
Txs struct is_tuple_<T<Xs...>> : std::true_type {};


// Return a typed nullptr
Tt T *null() { return (T*)nullptr; }


// Widening away from i8/u8 so we can print stuff easily
Tt If<sizeof(T) == 1, int> widen(T x) { return x; }
Tt If<sizeof(T) != 1, T>   widen(T x) { return x; }


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
Txs struct vc_
{
  Va<Xs...> v;

  template<class... Ys>
  operator Va<Ys...>() &&
    { return std::visit([](auto &&x) -> Va<Ys...> { return x; }, mo(v)); }
};

Txs vc_<Xs...> vc(Va<Xs...> &&v) { return {mo(v)}; }
Txs vc_<Xs...> vc(Va<Xs...> &v)  { return {v}; }


// Polymorphic functions
Txs struct fn : Xs... { using Xs::operator()...; };
Txs fn(Xs...) -> fn<Xs...>;


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


// Converting iterator
template<class I, class R> struct cit
{
  I i;
  bool  operator==(cit const &y) const { return i == y.i; }
  cit  &operator++()                   { ++i; return *this; }
  R     operator* ()             const { return *i; }
};

template<class T, class I, class R> struct citr
{
  T i;
  cit<I, R> begin() const { return {i.begin()}; }
  cit<I, R> end()   const { return {i.end()}; }
};


}


namespace std
{

Tn struct hash<τ::h224>
{
  size_t operator()(τ::h224 const &x) const
    {
      return hash<std::string_view>{}({(char const*) x.data(), x.size()});
    }
};

Tn struct hash<τ::h256>
{
  size_t operator()(τ::h256 const &x) const
    {
      return hash<std::string_view>{}({(char const*) x.data(), x.size()});
    }
};

Tn struct hash<τ::h384>
{
  size_t operator()(τ::h384 const &x) const
    {
      return hash<std::string_view>{}({(char const*) x.data(), x.size()});
    }
};

Tn struct hash<τ::h512>
{
  size_t operator()(τ::h512 const &x) const
    {
      return hash<std::string_view>{}({(char const*) x.data(), x.size()});
    }
};

}


#include "end.hh"


#endif
