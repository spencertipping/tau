#ifndef τπval_h
#define τπval_h


#include "types.hh"
#include "eta.hh"

#include "begin.hh"

namespace τ
{


struct πname final
{
  St n;
  operator Stc&() const { return n; }
};


struct πv;
struct πi;


// π expression used to compute a value (this is where C++ type
// and dependency erasure happens)

typedef F<πv(πi&)> πf;
typedef πf const   πfc;

typedef F<πv(πi&, πv&&)>             πmf;
typedef F<πv(πi&, πv&&, πv&&)>       πdf;
typedef F<πv(πi&, πv&&, πv&&, πv&&)> πtf;


struct πv final
{
  typedef Va<
    // η-mappable values (immediate, read-only)
    ηsig,
    i64,
    f64,
    St,
    πname,
    ηatom,
    ηi,
    Sn<i8bc>,
    Sn<i16bc>,
    Sn<i32bc>,
    Sn<i64bc>,
    Sn<f32bc>,
    Sn<f64bc>,

    // C++ native types (heap-allocated, mutable)
    Sp<V<i8>>,
    Sp<V<i16>>,
    Sp<V<i32>>,
    Sp<V<i64>>,
    Sp<V<f32>>,
    Sp<V<f64>>,

    Sp<Re>,
    Sp<V<πv>>,
    Sp<M<St, πv>>,
    Sp<πmf>,
    Sp<πdf>,
    Sp<πtf>
    > vt;

  // NOTE: these type mappings match ηi logic; they just allow for more
  // in-memory representations.
  sc ηtype πvts[] =
  {
    ηtype::sig,       // std::get<0>
    ηtype::n_int,
    ηtype::n_float,
    ηtype::string,
    ηtype::name,      // std::get<4>
    ηtype::atom,
    ηtype::η,
    ηtype::int8s,
    ηtype::int16s,    // std::get<8>
    ηtype::int32s,
    ηtype::int64s,
    ηtype::float32s,
    ηtype::float64s,  // std::get<12>

    ηtype::int8s,
    ηtype::int16s,
    ηtype::int32s,
    ηtype::int64s,    // std::get<16>
    ηtype::float32s,
    ηtype::float64s,

    ηtype::invalid,
    ηtype::invalid,   // std::get<20>
    ηtype::invalid,
    ηtype::invalid,
    ηtype::invalid,
    ηtype::invalid    // std::get<24>
  };


  vt v_;


  ηsig   sig    () const { return std::get<0>(v_); }
  i64    n_int  () const { return std::get<1>(v_); }
  f64    n_float() const { return std::get<2>(v_); }
  Stc   &str    () const { return std::get<3>(v_); }
  πname  name   () const { return std::get<4>(v_); }
  ηatom  atom   () const { return std::get<5>(v_); }
  ηi     η      () const { return std::get<6>(v_); }

  Sn<i8bc>  i8s () const { return std::get<7>(v_); }
  Sn<i16bc> i16s() const { return std::get<8>(v_); }
  Sn<i32bc> i32s() const { return std::get<9>(v_); }
  Sn<i64bc> i64s() const { return std::get<10>(v_); }
  Sn<f32bc> f32s() const { return std::get<11>(v_); }
  Sn<f64bc> f64s() const { return std::get<12>(v_); }

  Sp<V<i8>>     vi8s () const { return std::get<13>(v_); }
  Sp<V<i16>>    vi16s() const { return std::get<14>(v_); }
  Sp<V<i32>>    vi32s() const { return std::get<15>(v_); }
  Sp<V<i64>>    vi64s() const { return std::get<16>(v_); }
  Sp<V<f32>>    vf32s() const { return std::get<17>(v_); }
  Sp<V<f64>>    vf64s() const { return std::get<18>(v_); }

  Sp<Re>        re() const { return std::get<19>(v_); }
  Sp<V<πv>>     v()  const { return std::get<20>(v_); }
  Sp<M<St, πv>> m()  const { return std::get<21>(v_); }
  Sp<πmf>       mf() const { return std::get<22>(v_); }
  Sp<πdf>       df() const { return std::get<23>(v_); }
  Sp<πtf>       tf() const { return std::get<24>(v_); }


  ηtype     t() const { return πvts[v_.index()]; }
  bool native() const { return v_.index() > 12; }

  // NOTE: this mirrors ηi's <=> operator exactly, the only exceptions
  // being C++-hosted V<> and M<> variants, which are unordered since
  // we haven't committed them to η buffers.
  PO operator<=>(πv const&) const;

  bool operator< (πv const &x) const { return (*this <=> x) == PO::less; }
  bool operator> (πv const &x) const { return (*this <=> x) == PO::greater; }
  bool operator==(πv const &x) const { return (*this <=> x) == PO::equivalent; }
};

typedef πv const πvc;


O &operator<<(O&, πvc&);


}

#include "end.hh"


#endif
