#include "xi.hh"
#include "begin.hh"

namespace τ
{


#if τtrack_ξ
  static S<ξ*> ξs_;

  void ξc_(ξ *c) { ξs_.insert(c); }
  void ξd_(ξ *c) { ξs_.erase(c); }
  uN   ξn() { return ξs_.size(); }

  S<ξ*> &ξs() { return ξs_; }
#else
  static iN ξs_ = 0;

  void ξc_(ξ *c) { ++ξs_; }
  void ξd_(ξ *c) { A(--ξs_ >= 0, "ξs underflow"); }
  uN   ξn()      { return ξs_; }
#endif


uN ξallocated()
{
  return ζallocated();
}


bool ξ::eof(bool nonblock)
{
  while (!ra() && !wc) if (nonblock || !rg.y(λs::ξR)) return true;
  return !ra() && wc;
}


Sn<u8> ξ::iptr(uN size, bool nonblock)
{
  A(!s,   "ξ: iptr(" << size << ") with uncommitted value");
  A(size, "ξ: iptr() on size = 0");
  ensure(size + uNs);
  return reserve(size, nonblock);
}


void ξ::abort()
{
  A(s, "ξ: abort() without an active value");
  s = nullptr;
}

void ξ::commit(uN size)
{
  A(s,          "ξ: commit(" << size << ") without an active value");
  A(size <= *s, "ξ: commit(" << size << ") larger than reservation of " << *s);
  if (size) z.advance((*s = size) + uNs);
  else      z.advance(*s + uNs);
  wt += *s + uNs;
  s = nullptr;
  rg.w(true);  // we now have readable data
}


// Reader interface: delete ξ to finalize the read-side, iterator and
// unary */next() to read values
Sn<u8> ξ::operator*() const
{
  A(ra(), "*ξ with !ra(), " << z);
  return Sn<u8>(z + uNs, next_size());
}

void ξ::next()
{
  A(ra(), "++ξ with !ra(), " << z);
  z.free(next_size() + uNs);
  wg.w(true);  // we are now writable
}


Sn<u8> ξ::reserve(uN l, bool nb)
{
  for (;;)
    if (let r = z.iptr(l + uNs))
    {
      *(s = Rc<uN*>(r)) = l;
      return {r + uNs, l};
    }
    else if (nb || !wg.y(λs::ξW))
      return {Sc<u8*>(nullptr), 0};
}


O &operator<<(O &s, ξ const &y)
{
  return s << "ξ[" << (y.wc ? "#" : "")
           << "wt=" << y.wt << " " << y.z << "]";
}


}

#include "end.hh"
