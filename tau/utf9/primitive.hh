#ifndef tau_utf9_primitive_h
#define tau_utf9_primitive_h


#include <string>


#include "../types.hh"

#include "error-proto.hh"
#include "ibuf.hh"


#include "../module/begin.hh"

namespace tau::utf9
{

struct sym
{
  u64 h;
  sym(std::string const &s) : h(std::hash<std::string>{}(s)) {}
  sym(u64 h_)               : h(h_)                          {}
  operator u64() const { return h; }
};

struct hash
{
  u64 h;
  hash(u64 h_) : h(h_) {}
  operator u64() const { return h; }
  int compare(hash const x) const { return h > x.h ? 1 : h < x.h ? -1 : 0; }
};

struct pidfd
{
  u32 pid;
  u32 fd;

  pidfd(u32 pid_, u32 fd_) : pid(pid_), fd(fd_) {}

  operator u64() const { return (u64) pid << 32 | fd; }

  bool operator<(pidfd const &v) const { return pid < v.pid || pid == v.pid && fd < v.fd; }
  bool operator>(pidfd const &v) const { return pid > v.pid || pid == v.pid && fd > v.fd; }
};

struct greek
{
  enum letter : u8 { Α, Ι, Κ, Ρ, Θ, Τ, Ω };
  letter l;
  u32    v;

  greek(letter l_) : l(l_), v(0) {}
  greek(letter l_, u32 v_) : l(l_), v(v_) {}
  greek(ibuf const &b, u64 i)
    { switch(b.u8(i))
      {
      case 0x10: l = Α; v = 0; break;
      case 0x11: l = Ι; v = 0; break;
      case 0x12: l = Κ; v = 0; break;
      case 0x13: l = Ρ; v = b.u32(i + 1); break;
      case 0x14: l = Θ; v = b.u32(i + 1); break;
      case 0x15: l = Τ; v = 0; break;
      case 0x16: l = Ω; v = 0; break;
      default: throw_internal_error("greek bdec");
      } }

  bool has_v() const { return l == Ρ || l == Θ; }
  int compare(greek const x) const
    { return l == x.l ? (v > x.v ? 1 : v < x.v ? -1 : 0) : l > x.l ? 1 : -1; }
};


static_assert(sizeof(sym)   == sizeof(u64));
static_assert(sizeof(hash)  == sizeof(u64));
static_assert(sizeof(pidfd) == sizeof(u64));
static_assert(sizeof(greek) == sizeof(u64));

}

#include "../module/end.hh"


#endif
