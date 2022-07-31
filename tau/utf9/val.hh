#ifndef tau_utf9_val_h
#define tau_utf9_val_h


#include <cstdint>
#include <limits>
#include <string_view>
#include <vector>

#include "ibuf.hh"
#include "pfn.hh"
#include "primitive.hh"
#include "tval.hh"
#include "typecode.hh"


#include "../module/begin.hh"


namespace tau::utf9
{


// utf9 value, either bytecode-bound or immediate
// Instances of this fit into two machine words on 64-bit machines. As a result,
// we can pass them around by-value with no appreciable overhead (although we
// unfortunately have a destructor to consider).
struct val
{
  // ibuf const * will be 8-byte aligned, so tags with the low bit set are
  // type designators; see tagify() and tag_type().
  union
  { ibuf const * b;
    uint64_t     tag; } const;

  union
  { uint64_t                               i;
    uint64_t                               vu;
    int64_t                                vi;
    double                                 vd;
    float                                  vf;
    pidfd                                  vp;
    sym                                    vy;
    greek                                  vg;
    std::basic_string_view<uint8_t> const *vb;
    std::vector<val>                      *vt; } const;


  // Tagging bit schema:
  // type:6 | owned:1 | immediate:1
  static uint64_t const constexpr immed_bit = 1;
  static uint64_t const constexpr owned_bit = 2;
  static uint8_t  const constexpr tagshift  = 2;
  static_assert((immed_bit | owned_bit) >> tagshift == 0);

  static constexpr inline uint64_t tagify  (val_type t, bool own = false) { return static_cast<uint64_t>(t) << tagshift | (own ? owned_bit : 0) | immed_bit; }
  static constexpr inline val_type tag_type(uint64_t tag)                 { return static_cast<val_type>(tag >> tagshift); }


  // Immediate data structure ownership follows const-ness
  constexpr val(val const &v) : tag(v.tag & ~owned_bit), i(v.i) {}
            val(val &&v)      : tag(v.tag),              i(v.i) { v.tag &= ~owned_bit; }


  val(uint64_t tag_, uint64_t v_) : tag(tag_), vu(v_) {}

  val(ibuf const &b_, uint64_t i_) : b(&b_), i(i_)
    { if (reinterpret_cast<uint64_t>(b) & 7) throw internal_error("unaligned");
      b->check(i);
      b->check(i + b->len(i) - 1); }

  explicit val(uint64_t vu_) : tag(tagify(UINT64)), vu(vu_) {}
  explicit val(int64_t vi_)  : tag(tagify(INT64)),  vi(vi_) {}
  explicit val(uint32_t vu_) : tag(tagify(UINT32)), vu(vu_) {}
  explicit val(int32_t vi_)  : tag(tagify(INT32)),  vi(vi_) {}
  explicit val(uint16_t vu_) : tag(tagify(UINT16)), vu(vu_) {}
  explicit val(int16_t vi_)  : tag(tagify(INT16)),  vi(vi_) {}
  explicit val(uint8_t vu_)  : tag(tagify(UINT8)),  vu(vu_) {}
  explicit val(int8_t vi_)   : tag(tagify(INT8)),   vi(vi_) {}

  explicit val(double vd_)   : tag(tagify(FLOAT64)), vd(vd_) {}
  explicit val(float vf_)    : tag(tagify(FLOAT32)), vf(vf_) {}
  val(sym vy_)               : tag(tagify(SYMBOL)),  vy(vy_) {}
  val(pidfd vp_)             : tag(tagify(PIDFD)),   vp(vp_) {}
  val(greek vg_)             : tag(tagify(GREEK)),   vg(vg_) {}
  explicit val(bool b_)      : tag(tagify(BOOL)),    vu(b_)  {}

  val(std::vector<val> *vt_, val_type t_ = TUPLE)
    : tag(tagify(t_, true)),
      vt(vt_)
    { require_type(1ull << TUPLE | 1ull << ARRAY | 1ull << LIST | 1ull << SET | 1ull << MAP); }

  val(std::initializer_list<val> vs, val_type t_ = TUPLE)
    : tag(tagify(t_, true)),
      vt(new std::vector<val>(vs.begin(), vs.end()))
    { require_type(1ull << TUPLE | 1ull << ARRAY | 1ull << LIST | 1ull << SET | 1ull << MAP); }

  val(std::string const &s, val_type t_ = UTF8)
    : tag(tagify(t_, true)),
      vb(reinterpret_cast<std::basic_string_view<uint8_t>*>(
           new std::string_view(s.begin(), s.end())))
    { require_type(1ull << BYTES | 1ull << UTF8); }


  val(tval const &t_, ibuf const &b, uint64_t i)
    { switch (t_.typecode())
      {
      case 0x00: tag = tagify(UINT8);  vu = b.u8 (i); break;
      case 0x01: tag = tagify(UINT16); vu = b.u16(i); break;
      case 0x02: tag = tagify(UINT32); vu = b.u32(i); break;
      case 0x03: tag = tagify(UINT64); vu = b.u64(i); break;

      case 0x04: tag = tagify(INT8);  vi = b.i8 (i); break;
      case 0x05: tag = tagify(INT16); vi = b.i16(i); break;
      case 0x06: tag = tagify(INT32); vi = b.i32(i); break;
      case 0x07: tag = tagify(INT64); vi = b.i64(i); break;

      case 0x08: tag = tagify(FLOAT32); vf = b.f32(i); break;
      case 0x09: tag = tagify(FLOAT64); vd = b.f64(i); break;

      case 0x0a: tag = tagify(SYMBOL); vy = sym{b.u64(i)}; break;
      case 0x0b: tag = tagify(PIDFD);  vp = pidfd{b.u32(i), b.u32(i + 4)}; break;

      case 0x20: case 0x21: case 0x22: case 0x23:
      case 0x24: case 0x25: case 0x26: case 0x27:
      case 0x28: case 0x29: case 0x2a: case 0x2b:
      case 0x2c: case 0x2d: case 0x2e: case 0x2f:
      case 0x18:
      case 0x19:
      case 0x1a:
      case 0x1b: tag = tagify(UTF8, true); vb = new std::basic_string_view<uint8_t>(b + i, t_.vsize()); break;

      case 0x30: case 0x31: case 0x32: case 0x33:
      case 0x34: case 0x35: case 0x36: case 0x37:
      case 0x38: case 0x39: case 0x3a: case 0x3b:
      case 0x3c: case 0x3d: case 0x3e: case 0x3f:
      case 0x1c:
      case 0x1d:
      case 0x1e:
      case 0x1f: tag = tagify(BYTES, true); vb = new std::basic_string_view<uint8_t>(b + i, t_.vsize()); break;

      case 0x48: case 0x49: case 0x4a: case 0x4b:
      case 0x4c: case 0x4d: case 0x4e: case 0x4f:
      case 0x40:
      case 0x41:
      case 0x42:
      case 0x43:
        tag = tagify(TUPLE, true);
        vt  = new std::vector<val>;
        vt->reserve(t_.len());
        for (let &t : t_)
        { vt->push_back(val(t, b, i));
          i += t.vsize(); }
        break;

      case 0x44:
      case 0x45:
      case 0x46:
      case 0x47:
        // Unpack the array into the tuple representation, but track the fact
        // that it began as an array. We can reconstruct the array type by
        // examining the first element.
        tag = tagify(ARRAY, true);
        vt  = new std::vector<val>;
        vt->reserve(t_.len());
        for (uint64_t j = 0; j < t_.len(); j++)
        { vt->push_back(val(t_.atype(), b, i));
          i += t_.atype().vsize(); }
        break;

      default: throw internal_error("vtypecode");
      } }


  ~val()
    { if (tag & owned_bit)  // value is owned, so we need to free it
        // NOTE: if() above for performance; the switch below is slower and is
        // the uncommon case
        switch (tag)
        {
        case tagify(UTF8,  true):
        case tagify(BYTES, true): delete vb; break;

        case tagify(TUPLE, true):
        case tagify(ARRAY, true):
        case tagify(LIST,  true):
        case tagify(SET,   true):
        case tagify(MAP,   true): delete vt; break;
        } }


  // NOTE: &~2 is valid regardless of ibuf pointer status due to alignment
  val &operator=(val &&x)      { tag = x.tag;              i = x.i; x.tag &= ~owned_bit; return *this; }
  val &operator=(val const &x) { tag = x.tag & ~owned_bit; i = x.i;                      return *this; }


  bool     exists()                      const { return type() != NONE; }
  bool     has_ibuf()                    const { return !(tag & immed_bit); }
  val_type type()                        const { return has_ibuf() ? bts[b->u8(i)] : tag_type(tag); }
  bool     has_type(val_type_mask m)     const { return (1ull << type() & m); }
  void     require_ibuf()                const { if (!has_ibuf())  throw voperation_error("ibuf required", *this); }
  void     require_type(val_type_mask m) const { if (!has_type(m)) throw voperation_error("invalid type", *this); }
  uint64_t bsize()                       const { require_ibuf(); return b->len(i); }


  uint64_t mlen() const { return mend() - mbegin(); }

  uint8_t const *mbegin() const
    { if (has_ibuf()) return sfns[b->u8(i)](*b, i);
      require_type(1ull << UTF8 | 1ull << BYTES);
      return vb->data(); }

  uint8_t const *mend() const
    { if (has_ibuf()) return *b + i + b->len(i);
      require_type(1ull << UTF8 | 1ull << BYTES);
      return vb->data() + vb->size(); }

  uint64_t ibegin() const { require_ibuf(); return mbegin() - b->xs; }
  uint64_t iend()   const { require_ibuf(); return mend()   - b->xs; }


  struct it
  {
    union
    { ibuf               const * const b;
      std::vector<val>::const_iterator vi; };

    uint64_t i;

    it(ibuf const *b_, uint64_t i_)          : b(b_),   i(i_)   {}
    it(std::vector<val>::const_iterator vi_) : vi(vi_), i(-1ul) {}

    bool is_v() const { return i == -1ul; }

    val  operator*()               const { return is_v() ? *vi : val(*b, i); }
    it  &operator++()                    { if (is_v()) ++vi; else i += b->len(i); return *this; }
    bool operator==(it const &rhs) const { return b == rhs.b && i == rhs.i; }
  };

  it begin() const {
    require_type(1ull << TUPLE | 1ull << ARRAY);
    return has_ibuf() ? it(b, ibegin()) : it(vt->begin()); }

  it end() const {
    require_type(1ull << TUPLE | 1ull << ARRAY);
    return has_ibuf() ? it(b, iend()) : it(vt->end()); }


  uint64_t len() const
    { switch (tag)
      {
      case tagify(UTF8,  true):
      case tagify(BYTES, true): return vb->size();

      case tagify(ARRAY, true):
      case tagify(TUPLE, true): return vt->size();

      default:
      {
        let x = b->u8(i);
        if (x >= 0x48 && x <= 0x4f) return x - 0x48;
        if (x >= 0x54 && x <= 0x5f) return b->u16(i + 1) >> 1;
        if (x >= 0x64 && x <= 0x6f) return b->u32(i + 1) >> 2;
        if (x >= 0x74 && x <= 0x7f) return b->u64(i + 1) >> 3;
        switch (x)
        {
        case 0x18: case 0x1c: return b->u8 (i + 1);
        case 0x19: case 0x1d: return b->u16(i + 1);
        case 0x1a: case 0x1e: return b->u32(i + 1);
        case 0x1b: case 0x1f: return b->u64(i + 1);

        case 0x40: case 0x44: return b->u8 (i + 2);
        case 0x41: case 0x45: return b->u16(i + 3);
        case 0x42: case 0x46: return b->u32(i + 5);
        case 0x43: case 0x47: return b->u64(i + 9);

        case 0x50: return b->u16(i + 1) >> 1;
        case 0x51: return b->u32(i + 1) >> 2;
        case 0x52: return b->u64(i + 1) >> 3;
        case 0x53: return 0;

        default: throw voperation_error("len()", *this);
        } }
      } }


  uint64_t astride()        const { return atype().vsize(); }
  uint64_t asub(uint64_t i) const { return ibegin() + astride() * i; }

  tval atype() const
    { require_ibuf();  // FIXME: infer type from first tuple element if no ibuf
      require_type(1ull << ARRAY);
      switch (b->u8(i))
      {
      case 0x44: return tval(*b, i + 3);
      case 0x45: return tval(*b, i + 5);
      case 0x46: return tval(*b, i + 9);
      case 0x47: return tval(*b, i + 17);
      default: throw internal_error("val atype()");
      } }


  val target() const
    { require_type(1ull << LIST | 1ull << SET | 1ull << MAP);
      return has_ibuf() ? val(*b, ibegin()) : *this; }


  operator          sym()    const { require_type(1ull << SYMBOL);  return has_ibuf() ? sym{b->u64(i + 1)}                  : vy; }
  operator          pidfd()  const { require_type(1ull << PIDFD);   return has_ibuf() ? pidfd{b->u32(i + 1), b->u32(i + 5)} : vp; }
  operator          greek()  const { require_type(1ull << GREEK);   return has_ibuf() ? greek{*b, i}                        : vg;  }
  explicit operator bool()   const { require_type(1ull << BOOL);    return has_ibuf() ? b->u8(i) == 0x0d                    : !!vu; }
  operator          float()  const { require_type(1ull << FLOAT32); return has_ibuf() ? b->f32(i + 1)                       : vf; }
  operator          double() const { require_type(1ull << FLOAT64); return has_ibuf() ? b->f64(i + 1) : vd; }

  explicit operator uint64_t() const
    { if (has_type(1ull << INT8 | 1ull << INT16 | 1ull << INT32 | 1ull << INT64)) throw voperation_error("i->u", *this);
      require_type(1ull << UINT8 | 1ull << UINT16 | 1ull << UINT32 | 1ull << UINT64);
      if (!has_ibuf()) return vu;
      let x = b->u8(i);
      switch (x)
      {
      case 0x00: return b->u8 (i + 1);
      case 0x01: return b->u16(i + 1);
      case 0x02: return b->u32(i + 1);
      case 0x03: return b->u64(i + 1);
      default: throw internal_error("val u64");
      } }

  explicit operator int64_t() const
    { if (1ull << UINT8 | 1ull << UINT16 | 1ull << UINT32 | 1ull << UINT64) throw voperation_error("u->i", *this);
      require_type(1ull << INT8 | 1ull << INT16 | 1ull << INT32 | 1ull << INT64);
      if (!has_ibuf()) return vi;
      let x = b->u8(i);
      if (x >= 0x80) return x - 0x80;
      switch (x)
      {
      case 0x04: return b->i8 (i + 1);
      case 0x05: return b->i16(i + 1);
      case 0x06: return b->i32(i + 1);
      case 0x07: return b->i64(i + 1);
      default: throw internal_error("val i64");
      } }

  operator std::string_view() const
    { require_type(1ull << UTF8 | 1ull << BYTES);
      return has_ibuf()
        ? std::string_view(reinterpret_cast<char const*>(mbegin()), mlen())
        : *reinterpret_cast<std::string_view const*>(vb); }


  hash h() const
    { let t = type();
      switch (t)
      {

#define ht(ct)                                                          \
        { let x = ce(static_cast<uint64_t>(static_cast<ct>(*this)));    \
          return xxh(&x, sizeof x, t); }
      case UINT64: case UINT32: case UINT16: case UINT8: ht(uint64_t)
      case INT64:  case INT32:  case INT16:  case INT8:  ht(int64_t)
      case SYMBOL:  ht(sym)
      case PIDFD:   ht(pidfd)
#undef ht

      case FLOAT64: { let x = ce(static_cast<double>(*this)); return xxh(&x, sizeof x, t); }
      case FLOAT32: { let x = ce(static_cast<float> (*this)); return xxh(&x, sizeof x, t); }

      case BOOL: return xxh(NULL, 0, static_cast<bool>(*this) ? 0x0d : 0x0c);

      case NULLTYPE: return xxh(NULL, 0, t);
      case GREEK: { let g = static_cast<greek>(*this); return xxc(g.l, g.v); }

      case UTF8:
      case BYTES: return xxh(mbegin(), mend() - mbegin(), t);

      case TUPLE:
      case LIST:
      case SET:
      case MAP:
      { uint64_t h = xxh(NULL, 0, t);
        for (let &v : *this) h = xxc(v.h().h, h);
        return h; }

      default: throw voperation_error("h", *this);
      } }


  int compare(val const &v) const
    { let t1 =   type();
      let t2 = v.type();
      if (t1 != t2) throw binop_error("ct", *this, v);
      switch (t1)
      {

#define cmpblock(t)                                     \
        { let x1 = static_cast<t>(*this);               \
          let x2 = static_cast<t>(v);                   \
          return x1 > x2 ? 1 : x1 < x2 ? -1 : 0; }
      case UINT64: case UINT32: case UINT16: case UINT8: cmpblock(uint64_t)
      case INT64:  case INT32:  case INT16:  case INT8:  cmpblock(int64_t)
      case FLOAT32: cmpblock(float)
      case FLOAT64: cmpblock(double)
      case SYMBOL:  cmpblock(sym)
      case PIDFD:   cmpblock(pidfd)
      case BOOL:    cmpblock(bool)
#undef cmpblock

      case NULLTYPE: return 0;
      case GREEK:    return static_cast<greek>(*this).compare(static_cast<greek>(v));

      case UTF8:
      case BYTES:
      { if (let tc = atype().compare(v.atype())) throw binop_error("cta", *this, v);
        let n1 =   mend() -   mbegin();
        let n2 = v.mend() - v.mbegin();
        if (let c = std::memcmp(mbegin(), v.mbegin(), std::min(n1, n2))) return c;
        return n1 > n2 ? 1 : n1 < n2 ? -1 : 0; }

      case TUPLE:
      { bool m1 = false, m2 = false;
        for (it i1 = begin(), i2 = v.begin(), e1 = end(), e2 = v.end();
             m1 = i1 != e1, m2 = i2 != e2, m1 && m2;
             ++i1, ++i2)
          if (let c = (*i1).compare(*i2)) return c;
        return m1 ? 1 : m2 ? -1 : 0; }

      default: throw binop_error("nc", *this, v);
      } }

  bool operator< (val const &v) const { return this->compare(v) < 0; }
  bool operator> (val const &v) const { return this->compare(v) > 0; }
  bool operator<=(val const &v) const { return this->compare(v) <= 0; }
  bool operator>=(val const &v) const { return this->compare(v) >= 0; }

  bool operator==(val const &v) const { return type() == v.type() && this->compare(v) == 0; }
  bool operator!=(val const &v) const { return type() != v.type() || this->compare(v) != 0; }


  val &operator<<(val const &v)
    { require_type(1ull << TUPLE);
      if (has_ibuf()) throw voperation_error("val<< +ibuf", *this);
      vt->push_back(v);
      return *this; }


  val operator[](uint64_t i) const
    { switch(type())
      {
      case BYTES: return val(mbegin()[i]);
      case TUPLE: return tp(i);
      case ARRAY: return ap(i);
      case LIST:  return lp(i);
      default:    throw voperation_error("v[u64]", *this);
      } }

  val operator[](val const &v) const
    { throw voperation_error("v[v] TODO", *this); }


  // NOTE: immediate vals are never arrays (since there isn't space to store
  // the array type + tag + vector). So this will be called only with an ibuf.
  val ap(uint64_t i) const { require_ibuf(); return val(atype(), *b, asub(i)); }

  // All functions below use hinting: hi or hk to indicate the thing that was
  // hinted, and h to indicate the byte offset (beyond mbegin()) to start
  // looking. In every case, h <= offset and hk <= k (or for hashed, H[hk] <
  // H[k]); decoding is forward-only, so hints can't overshoot.
  val tp(uint64_t i, uint64_t hi = 0, uint64_t h = 0) const;

  val lp(uint64_t i) const { throw voperation_error("l[u64] TODO", *this); }


  // te = tuple element (tuple-as-set)
  // tk = tuple key (tuple-as-map)
  // to = tuple ordered (indexed things in sorted order)
  // th = tuple hashed (indexed things sorted by hash)
  struct kf_te { val const &operator()(val const &e)  { return e; } };
  struct kf_tk { val const  operator()(val const &kv) { return kv[0]; } };

  template <class KF> val  ih(val const &k) const;
  template <class KF> val  th(val const &k, val const &hk, uint64_t h = 0) const;
  template <class KF> bool is_th() const;
  template <class KF> val  make_th() const;
};


val const none(val::tagify(NONE), 0);

val const α(val(greek{greek::Α}));
val const ω(val(greek{greek::Ω}));
val const ι(val(greek{greek::Ι}));
val const κ(val(greek{greek::Κ}));
val const τ(val(greek{greek::Τ}));

inline val ρ(uint32_t x) { return val(greek{greek::Ρ, x}); }
inline val θ(uint32_t x) { return val(greek{greek::Θ, x}); }
inline val θ(double   x) { return val(greek{greek::Θ, static_cast<uint32_t>(x * static_cast<double>(std::numeric_limits<uint32_t>::max()))}); }

inline val u9t(uint64_t n = 0) { let vt = new std::vector<val>; if (n) vt->reserve(n); return val(vt); }
inline val u9a(uint64_t n = 0) { let vt = new std::vector<val>; if (n) vt->reserve(n); return val(vt, ARRAY); }
inline val u9l(uint64_t n = 0) { let vt = new std::vector<val>; if (n) vt->reserve(n); return val(vt, LIST); }
inline val u9s(uint64_t n = 0) { let vt = new std::vector<val>; if (n) vt->reserve(n); return val(vt, SET); }
inline val u9m(uint64_t n = 0) { let vt = new std::vector<val>; if (n) vt->reserve(n); return val(vt, MAP); }


inline oenc &tval::pack(oenc &o, val const &v) const
{
  switch (type())
  {
  case UINT8:  return o.u8(cou8(static_cast<uint64_t>(v)));
  case UINT16: return o.u16(cou16(static_cast<uint64_t>(v)));
  case UINT32: return o.u32(cou32(static_cast<uint64_t>(v)));
  case UINT64: return o.u64(static_cast<uint64_t>(v));

  case INT8:   return o.u8(coi8(static_cast<int64_t>(v)));
  case INT16:  return o.u16(coi16(static_cast<int64_t>(v)));
  case INT32:  return o.u32(coi32(static_cast<int64_t>(v)));
  case INT64:  return o.u64(static_cast<int64_t>(v));

  case FLOAT32: return o.f32(static_cast<float>(v));
  case FLOAT64: return o.f64(static_cast<double>(v));
  case SYMBOL:  return o.u64(static_cast<uint64_t>(v));
  case PIDFD:   return o.u32(static_cast<pidfd>(v).pid).u32(static_cast<pidfd>(v).fd);

  case UTF8:
  case BYTES:
  { let l = v.mlen(), s = vsize();
    o.xs(v.mbegin(), std::min(l, s)).fill(0, l > s ? 0 : s - l);
    return o; }

  case TUPLE:
  { tval::it ti(begin()),   te(end());
    val::it  vi(v.begin()), ve(v.end());
    while (ti != te && vi != ve) (*ti).pack(o, *vi), ++ti, ++vi;
    if ((ti != te) != (vi != ve)) throw encoding_error("t vn>tn", *this, v);
    return o; }

  case ARRAY:
  { let      at = atype();
    uint64_t n  = len();
    val::it  vi(v.begin()), ve(v.end());
    while (n && vi != ve) at.pack(o, *vi), ++vi, n--;
    if (n || vi != ve) throw encoding_error("a vn>tn", *this, v);
    return o; }

  case LIST:
  case SET:
  case MAP: return pack(o, v.target());

  default: throw encoding_error("ns", *this, v);
  }
}


}


template<> struct std::hash<tau::utf9::val>
{ uint64_t operator()(tau::utf9::val const &v) { return v.h(); } };

template<> struct std::hash<tau::utf9::tval>
{ uint64_t operator()(tau::utf9::tval const &t) { return t.h(); } };


#include "../module/end.hh"


#endif
