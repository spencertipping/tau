#ifndef tau_utf9_val_h
#define tau_utf9_val_h


#include <string_view>


#include "../types.hh"

#include "error-proto.hh"
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
  typedef uN tag_t;


  // ibuf const * will be at least 4-byte aligned, so tags with the low bit set
  // are type designators; see tagify() and tag_type().
  union
  { ibuf const * b;
    tag_t        tag;
    u64          _this_field_forces_val_size_to_16_bytes; } const;

  union
  { uN        i;
    u64       vu;
    i64       vi;
    f64       vd;
    f32       vf;
    pidfd     vp;
    sym       vy;
    greek     vg;
    Bv const *vb;
    V<val>   *vt; } const;


  // Tagging bit schema:
  // type:6 | owned:1 | immediate:1
  // NOTE: 32-bit builds are just 4-byte aligned, so these are all the bits
  // we have to work with
  static tag_t const constexpr immed_bit = 1;
  static tag_t const constexpr owned_bit = 2;
  static tag_t const constexpr tagmask   = immed_bit | owned_bit;
  static u8    const constexpr tagshift  = 2;
  static_assert(tagmask >> tagshift == 0);

  static constexpr inline tag_t    tagify  (val_type t, bool own = false) { return Sc<tag_t>(t) << tagshift | (own ? owned_bit : 0) | immed_bit; }
  static constexpr inline val_type tag_type(tag_t tag)                    { return Sc<val_type>(tag >> tagshift); }


  // Immediate data structure ownership follows const-ness
  constexpr val(val const &v) : tag(v.tag & ~owned_bit), vu(v.vu) {}
            val(val &&v)      : tag(v.tag),              vu(v.vu) { v.tag &= ~owned_bit; }


  val(tag_t tag_, u64 v_) : tag(tag_), vu(v_) {}

  val(ibuf const &b_, uN i_) : b(&b_), i(i_)
    { if (Rc<uN>(b) & tagmask) throw_internal_error("unaligned");
      b->check(i);
      b->check(i + b->len(i) - 1); }

  explicit val(u64 vu_, val_type t_ = UINT) : tag(tagify(t_)), vu(vu_) {}
  explicit val(i64 vi_, val_type t_ = INT)  : tag(tagify(t_)), vi(vi_) {}

  explicit val(f64 vd_)   : tag(tagify(FLOAT64)), vd(vd_) {}
  explicit val(f32 vf_)   : tag(tagify(FLOAT32)), vf(vf_) {}
  explicit val(sym vy_)   : tag(tagify(SYMBOL)),  vy(vy_) {}
  explicit val(pidfd vp_) : tag(tagify(PIDFD)),   vp(vp_) {}
  explicit val(greek vg_) : tag(tagify(GREEK)),   vg(vg_) {}
  explicit val(bool b_)   : tag(tagify(BOOL)),    vu(b_)  {}

  val(V<val> *vt_, val_type t_ = TUPLE) : tag(tagify(t_, true)), vt(vt_)
    { require_type(multi_types); }

  val(Il<val> vs, val_type t_ = TUPLE) : tag(tagify(t_, true)),
                                         vt(new V<val>(vs.begin(), vs.end()))
    { require_type(multi_types); }

  val(std::string const &s, val_type t_ = UTF8) : tag(tagify(t_, true)),
                                                  vb(Rc<Bv*>(new std::string_view(s.begin(), s.end())))
    { require_type(1ull << BYTES | 1ull << UTF8); }


  val(tval const &t_, ibuf const &b_, uN i)
    { b_.check(i);
      b_.check(i + t_.vsize() - 1);
      switch (t_.typecode())
      {
      case 0x00: tag = tagify(UINT8);  vu = b_.U8 (i); break;
      case 0x01: tag = tagify(UINT16); vu = b_.U16(i); break;
      case 0x02: tag = tagify(UINT32); vu = b_.U32(i); break;
      case 0x03: tag = tagify(UINT64); vu = b_.U64(i); break;

      case 0x04: tag = tagify(INT8);  vi = b_.I8 (i); break;
      case 0x05: tag = tagify(INT16); vi = b_.I16(i); break;
      case 0x06: tag = tagify(INT32); vi = b_.I32(i); break;
      case 0x07: tag = tagify(INT64); vi = b_.I64(i); break;

      case 0x08: tag = tagify(FLOAT32); vf = b_.F32(i); break;
      case 0x09: tag = tagify(FLOAT64); vd = b_.F64(i); break;

      case 0x0a: tag = tagify(SYMBOL); vy = sym{b_.U64(i)}; break;
      case 0x0b: tag = tagify(PIDFD);  vp = pidfd{b_.U32(i), b_.U32(i + 4)}; break;

      case 0x20: case 0x21: case 0x22: case 0x23:
      case 0x24: case 0x25: case 0x26: case 0x27:
      case 0x28: case 0x29: case 0x2a: case 0x2b:
      case 0x2c: case 0x2d: case 0x2e: case 0x2f:
      case 0x18:
      case 0x19:
      case 0x1a:
      case 0x1b: tag = tagify(UTF8, true); vb = new Bv(b_ + i, t_.vsize()); break;

      case 0x30: case 0x31: case 0x32: case 0x33:
      case 0x34: case 0x35: case 0x36: case 0x37:
      case 0x38: case 0x39: case 0x3a: case 0x3b:
      case 0x3c: case 0x3d: case 0x3e: case 0x3f:
      case 0x1c:
      case 0x1d:
      case 0x1e:
      case 0x1f: tag = tagify(BYTES, true); vb = new Bv(b_ + i, t_.vsize()); break;

      case 0x48: case 0x49: case 0x4a: case 0x4b:
      case 0x4c: case 0x4d: case 0x4e: case 0x4f:
      case 0x40:
      case 0x41:
      case 0x42:
      case 0x43:
        tag = tagify(TUPLE, true);
        vt  = new V<val>;
        vt->reserve(t_.len());
        for (let &t : t_)
        { vt->push_back(val(t, b_, i));
          i += t.vsize(); }
        break;

      case 0x44:
      case 0x45:
      case 0x46:
      case 0x47:
      { // Unpack the array into the tuple representation, but track the fact
        // that it began as an array. We can reconstruct the array type by
        // examining the first element.
        tag = tagify(ARRAY, true);
        let at = t_.atype();
        let n  = t_.len();
        let s  = at.vsize();
        vt = new V<val>;
        vt->reserve(n);
        for (uN j = 0; j < n; j++) { vt->push_back(val(at, b_, i)); i += s; }
        break; }

      default: throw_internal_error("vtypecode");
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


  // NOTE: &~owned_bit is valid regardless of ibuf pointer status due to alignment
  val &operator=(val &&x)      { tag = x.tag;              i = x.i; x.tag &= ~owned_bit; return *this; }
  val &operator=(val const &x) { tag = x.tag & ~owned_bit; i = x.i;                      return *this; }


  bool     exists()                      const { return type() != NONE; }
  bool     has_ibuf()                    const { return !(tag & immed_bit); }
  val_type type()                        const { return has_ibuf() ? vts[b->U8(i)] : tag_type(tag); }
  bool     has_type(val_type_mask m)     const { return (1ull << type() & m); }
  void     require_ibuf()                const { if (!has_ibuf())  throw_vop_error("ibuf required", *this); }
  void     require_type(val_type_mask m) const { if (!has_type(m)) throw_vop_error("invalid type", *this); }
  uN       bsize()                       const { require_ibuf(); return b->len(i); }


  uN mlen() const { return mend() - mbegin(); }

  u8c *mbegin() const
    { if (has_ibuf()) return sfns[b->U8(i)](*b, i);
      require_type(1ull << UTF8 | 1ull << BYTES);
      return vb->data(); }

  u8c *mend() const
    { if (has_ibuf()) return *b + (i + b->len(i));
      require_type(1ull << UTF8 | 1ull << BYTES);
      return vb->data() + vb->size(); }

  uN ibegin() const { require_ibuf(); return mbegin() - b->xs; }
  uN iend()   const { require_ibuf(); return mend()   - b->xs; }


  struct it
  {
    enum kind { VEC = 1, IBUF_TUPLE = 2, IBUF_ARRAY = 3 };

    union { ibuf const *buf; u64 tag; };
    union { Vi<val>     vi;  uN  i; };
    tval atype;
    uN   stride;

    it(Vi<val> vi_)           : buf(nullptr), i(0),  atype(tu8) { vi = vi_; tag |= VEC; }
    it(ibuf const *b_, uN i_) : buf(b_),      i(i_), atype(tu8) {           tag |= IBUF_TUPLE; }
    it(ibuf const *b_, tval const &atype_, uN i_)
      : buf(b_), i(i_), atype(atype_), stride(atype.vsize()) { tag |= IBUF_ARRAY; }

    bool operator==(it const &x) const { return i == x.i; }

    ibuf const &b() const { return *Rc<ibuf const *>(Rc<uN>(buf) & ~3ull); }

    val operator*() const
      { switch (tag & 3)
        {
        case VEC:        return *vi;
        case IBUF_TUPLE: return val(b(), i);
        case IBUF_ARRAY: return val(atype, b(), i);
        default:         return throw_badbyte_error<val>("it*", tag & 3);
        } }

    it &operator++()
      { switch (tag & 3)
        {
        case VEC:        ++vi;            break;
        case IBUF_TUPLE: i += b().len(i); break;
        case IBUF_ARRAY: i += stride;     break;
        default:         return throw_badbyte_error<it&>("it++", tag & 3);
        }
        return *this; }
  };


  it begin() const
    { require_type(multi_types);
      return !has_ibuf() ? it(vt->begin()) : type() == ARRAY ? it(b, atype(), ibegin()) : it(b, ibegin()); }

  it end() const
    { require_type(multi_types);
      return !has_ibuf() ? it(vt->end())   : type() == ARRAY ? it(b, atype(), iend())   : it(b, iend()); }


  uN len() const
    { switch (tag)
      {
      case tagify(UTF8,  true):
      case tagify(BYTES, true): return vb->size();

      case tagify(ARRAY, true):
      case tagify(LIST, true):
      case tagify(MAP, true):
      case tagify(SET, true):
      case tagify(TUPLE, true): return vt->size();

      default:
      { require_ibuf();
        let x = b->U8(i);
        if (x >= 0x48 && x <= 0x4f) return x - 0x48;
        switch (x)
        {
        case 0x18: case 0x1c: return b->U8 (i + 1);
        case 0x19: case 0x1d: return b->U16(i + 1);
        case 0x1a: case 0x1e: return b->U32(i + 1);
        case 0x1b: case 0x1f: return b->U64(i + 1);

        case 0x40: case 0x44: return b->U8 (i + 2);
        case 0x41: case 0x45: return b->U16(i + 3);
        case 0x42: case 0x46: return b->U32(i + 5);
        case 0x43: case 0x47: return b->U64(i + 9);

        default: return throw_vop_error<uN>("len()", *this);
        } }
      } }


  uN astride()  const { return atype().vsize(); }
  uN asub(uN i) const { return ibegin() + astride() * i; }

  tval atype() const
    { require_type(1ull << ARRAY);
      if (!has_ibuf()) return len() ? (*this)[0].inferred_type() : Sc<tval>(tu8);
      switch (b->U8(i))
      {
      case 0x44: return tval(*b, i + 3);
      case 0x45: return tval(*b, i + 5);
      case 0x46: return tval(*b, i + 9);
      case 0x47: return tval(*b, i + 17);
      default: return throw_internal_error<tval>("val atype()");
      } }

  tval inferred_type()  const { return static_cast<tval>(inferred_type_()); }
  tbuf inferred_type_() const
    { switch(type())
      {
      case UINT8:   return tu8;
      case UINT16:  return tu16;
      case UINT32:  return tu32;
      case UINT64:  return tu64;
      case INT8:    return ti8;
      case INT16:   return ti16;
      case INT32:   return ti32;
      case INT64:   return ti64;
      case FLOAT32: return tf32;
      case FLOAT64: return tf64;
      case SYMBOL:  return tsym;
      case PIDFD:   return tpidfd;
      case UTF8:    return tutf8(len());
      case BYTES:   return tbytes(len());

      case ARRAY: { let n = len(); return tarray(n, n > 0 ? (*this)[0].inferred_type_() : tu8); }

      case TUPLE:
      case LIST:
      case SET:
      case MAP:
      { V<tbuf> ts;
        ts.reserve(len());
        uN i = 0;
        for (let &x : *this) ts[i++] = x.inferred_type_();
        return ttuple(ts); }

      default: return throw_vop_error<tbuf>("infer_type", *this);
      } }


  val target() const
    { require_type(1ull << LIST | 1ull << SET | 1ull << MAP);
      return has_ibuf() ? val(*b, ibegin()) : *this; }


  operator          sym()    const { require_type(1ull << SYMBOL);  return has_ibuf() ? sym{b->U64(i + 1)}                  : vy; }
  operator          pidfd()  const { require_type(1ull << PIDFD);   return has_ibuf() ? pidfd{b->U32(i + 1), b->U32(i + 5)} : vp; }
  operator          greek()  const { require_type(1ull << GREEK);   return has_ibuf() ? greek{*b, i}                        : vg;  }
  explicit operator bool()   const { require_type(1ull << BOOL);    return has_ibuf() ? b->U8(i) == 0x0d                    : !!vu; }
  operator          float()  const { require_type(1ull << FLOAT32); return has_ibuf() ? b->F32(i + 1)                       : vf; }
  operator          double() const { require_type(1ull << FLOAT64); return has_ibuf() ? b->F64(i + 1) : vd; }

  explicit operator u64() const
    { if (has_type(int_types)) throw_vop_error("i->u", *this);
      require_type(uint_types);
      if (!has_ibuf()) return vu;
      let x = b->U8(i);
      switch (x)
      {
      case 0x00: return b->U8 (i + 1);
      case 0x01: return b->U16(i + 1);
      case 0x02: return b->U32(i + 1);
      case 0x03: return b->U64(i + 1);
      default: return throw_internal_error<u64>("val u64");
      } }

  explicit operator i64() const
    { if (has_type(uint_types)) throw_vop_error("u->i", *this);
      require_type(int_types);
      if (!has_ibuf()) return vi;
      let x = b->U8(i);
      if (x >= 0x80) return x - 0x80;
      switch (x)
      {
      case 0x04: return b->I8 (i + 1);
      case 0x05: return b->I16(i + 1);
      case 0x06: return b->I32(i + 1);
      case 0x07: return b->I64(i + 1);
      default: return throw_internal_error<i64>("val i64");
      } }

  operator std::string_view() const
    { require_type(1ull << UTF8 | 1ull << BYTES);
      return has_ibuf()
        ? std::string_view(Rc<char const*>(mbegin()), mlen())
        : *Rc<std::string_view const*>(vb); }

  val coerce(val_type t_) const { return val{tagify(t_), vu}; }


  hash h() const
    { let t = type();
      switch (t)
      {

#define ht(ct, t_) { let x = ce(Sc<u64>(Sc<ct>(*this))); return xxh(&x, sizeof x, t_); }
      case UINT: case UINT64: case UINT32: case UINT16: case UINT8: ht(u64, UINT)
      case INT:  case INT64:  case INT32:  case INT16:  case INT8:  ht(i64, INT)
      case SYMBOL: ht(sym,   SYMBOL)
      case PIDFD:  ht(pidfd, PIDFD)
#undef ht

      case FLOAT64: { let x = ce(Sc<f64>(*this)); return xxh(&x, sizeof x, t); }
      case FLOAT32: { let x = ce(Sc<f32>(*this)); return xxh(&x, sizeof x, t); }

      case BOOL: return xxh(NULL, 0, Sc<bool>(*this) ? 0x0d : 0x0c);

      case NULLTYPE: return xxh(NULL, 0, t);
      case GREEK: { let g = Sc<greek>(*this); return xxc(g.l, g.v); }

      case UTF8:
      case BYTES: return xxh(mbegin(), mend() - mbegin(), t);

      case TUPLE:
      case ARRAY:
      case LIST:
      case SET:
      case MAP:
      { u64 h = xxh(NULL, 0, t);
        for (let &v : *this) h = xxc(v.h().h, h);
        return h; }

      default: return throw_vop_error<hash>("h", *this);
      } }


  int compare(val const &v) const
    { let t1 =   type();
      let t2 = v.type();
      let m1 = 1ull << t1;
      let m2 = 1ull << t2;

#define cmpblock(t)                                     \
      { let x1 = Sc<t>(*this);                          \
        let x2 = Sc<t>(v);                              \
        return x1 > x2 ? 1 : x1 < x2 ? -1 : 0; }

      if (m1 &  uint_types && m2 &  uint_types) cmpblock(u64);
      if (m1 &   int_types && m2 &   int_types) cmpblock(i64);
      if (m1 & float_types && m2 & float_types) cmpblock(f64);

      if (t1 != t2) throw_binop_error("ct", *this, v);

      switch (t1)
      {

      case SYMBOL:  cmpblock(sym)
      case PIDFD:   cmpblock(pidfd)
      case BOOL:    cmpblock(bool)
#undef cmpblock

      case NULLTYPE: return 0;
      case GREEK:    return Sc<greek>(*this).compare(Sc<greek>(v));

      case UTF8:
      case BYTES:
      { if (let tc = atype().compare(v.atype())) return throw_binop_error<int>("cta", *this, v);
        let n1 =   mend() -   mbegin();
        let n2 = v.mend() - v.mbegin();
        if (let c = std::memcmp(mbegin(), v.mbegin(), std::min(n1, n2))) return c;
        return n1 > n2 ? 1 : n1 < n2 ? -1 : 0; }

      case TUPLE:
      case ARRAY:
      case LIST:
      case SET:
      case MAP:
      { bool m1 = false, m2 = false;
        for (it i1 = begin(), i2 = v.begin(), e1 = end(), e2 = v.end();
             m1 = i1 != e1, m2 = i2 != e2, m1 && m2;
             ++i1, ++i2)
          if (let c = (*i1).compare(*i2)) return c;
        return m1 ? 1 : m2 ? -1 : 0; }

      default: return throw_binop_error<int>("nc", *this, v);
      } }

  bool operator< (val const &v) const { return this->compare(v) < 0; }
  bool operator> (val const &v) const { return this->compare(v) > 0; }
  bool operator<=(val const &v) const { return this->compare(v) <= 0; }
  bool operator>=(val const &v) const { return this->compare(v) >= 0; }

  bool operator==(val const &v) const { return type() == v.type() && this->compare(v) == 0; }
  bool operator!=(val const &v) const { return type() != v.type() || this->compare(v) != 0; }


  val &operator<<(val const &v)
    { require_type(1ull << TUPLE);
      if (has_ibuf()) return throw_vop_error<val&>("val<< +ibuf", *this);
      vt->push_back(v);
      return *this; }


  val operator[](uN i) const
    { switch (type())
      {
      case BYTES: return val(Sc<u64>(mbegin()[i]), UINT8);
      case TUPLE: return tp(i);
      case ARRAY: return ap(i);
      case LIST:  return lp(i);
      default:    return throw_vop_error<val>("v[uN]", *this);
      } }

  val operator[](val const &v) const
    { return throw_vop_error<val>("v[v] TODO", *this); }


  val ap(uN i) const
    { return has_ibuf() ? val(atype(), *b, asub(i)) : (*vt)[i]; }

  // Hinting: hi or hk to indicate the thing that was hinted, and h to indicate
  // the byte offset (beyond mbegin()) to start looking. In every case, h <=
  // offset and hk <= k (or for hashed, H[hk] < H[k]); decoding is forward-only,
  // so hints can't overshoot.
  val tp(uN i, uN hi = 0, uN h = 0) const;

  val lp(uN i) const { return throw_vop_error<val>("l[uN] TODO", *this); }


  // te = tuple element (tuple-as-set)
  // tk = tuple key (tuple-as-map)
  // to = tuple ordered (indexed things in sorted order)
  // th = tuple hashed (indexed things sorted by hash)
  struct kf_te { val const &operator()(val const &e)  { return e; } };
  struct kf_tk { val const  operator()(val const &kv) { return kv[0]; } };

  template <class KF> val  ih(val const &k) const;
  template <class KF> val  th(val const &k, val const &hk, uN h = 0) const;
  template <class KF> bool is_th() const;
  template <class KF> val  make_th() const;
};


val const u9n(val::tagify(NONE), 0);

val const α(val(greek{greek::Α}));
val const ω(val(greek{greek::Ω}));
val const ι(val(greek{greek::Ι}));
val const κ(val(greek{greek::Κ}));
val const τ(val(greek{greek::Τ}));

inline val ρ(u32 x) { return val(greek{greek::Ρ, x}); }
inline val θ(u32 x) { return val(greek{greek::Θ, x}); }
inline val θ(f64 x) { return val(greek{greek::Θ, Sc<u32>(x * Sc<f64>(Nl<u32>::max()))}); }

inline val u9t(uN n = 0) { let vt = new V<val>; if (n) vt->reserve(n); return val(vt); }
inline val u9a(uN n = 0) { let vt = new V<val>; if (n) vt->reserve(n); return val(vt, ARRAY); }
inline val u9l(uN n = 0) { let vt = new V<val>; if (n) vt->reserve(n); return val(vt, LIST); }
inline val u9s(uN n = 0) { let vt = new V<val>; if (n) vt->reserve(n); return val(vt, SET); }
inline val u9m(uN n = 0) { let vt = new V<val>; if (n) vt->reserve(n); return val(vt, MAP); }


inline oenc &tval::pack(oenc &o, val const &v) const
{
  switch (type())
  {
  case UINT8:  return o.U8(cou8(Sc<u64>(v)));
  case UINT16: return o.U16(cou16(Sc<u64>(v)));
  case UINT32: return o.U32(cou32(Sc<u64>(v)));
  case UINT64:
  case UINT:   return o.U64(Sc<u64>(v));

  case INT8:   return o.U8(coi8(Sc<i64>(v)));
  case INT16:  return o.U16(coi16(Sc<i64>(v)));
  case INT32:  return o.U32(coi32(Sc<i64>(v)));
  case INT64:
  case INT:    return o.U64(Sc<i64>(v));

  case FLOAT32: return o.F32(Sc<f64>(v));
  case FLOAT64: return o.F64(Sc<f32>(v));
  case SYMBOL:  return o.U64(Sc<u64>(v));
  case PIDFD:   return o.U32(Sc<pidfd>(v).pid).U32(static_cast<pidfd>(v).fd);

  case UTF8:
  case BYTES:
  { let l = v.mlen(), s = vsize();
    o.xs(v.mbegin(), std::min(l, s)).fill(0, l > s ? 0 : s - l);
    return o; }

  case TUPLE:
  { tval::it ti(begin()),   te(end());
    val::it  vi(v.begin()), ve(v.end());
    while (ti != te && vi != ve) (*ti).pack(o, *vi), ++ti, ++vi;
    if ((ti != te) != (vi != ve)) return throw_encoding_error<oenc&>("t vn>tn", *this, v);
    return o; }

  case ARRAY:
  { let     at = atype();
    uN      n  = len();
    val::it vi(v.begin()), ve(v.end());
    while (n && vi != ve) at.pack(o, *vi), ++vi, n--;
    if (n || vi != ve) return throw_encoding_error<oenc&>("a vn>tn", *this, v);
    return o; }

  case LIST:
  case SET:
  case MAP: return pack(o, v.target());

  default: return throw_encoding_error<oenc&>("pack ns", *this, v);
  }
}


}


template<> struct std::hash<tau::utf9::val>
{ uint64_t operator()(tau::utf9::val const &v) { return v.h(); } };

template<> struct std::hash<tau::utf9::tval>
{ uint64_t operator()(tau::utf9::tval const &t) { return t.h(); } };


#include "../module/end.hh"


#endif
