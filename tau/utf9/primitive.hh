#ifndef tau_utf9_primitive_h
#define tau_utf9_primitive_h


#include <cstdint>
#include <string>

#include "error-proto.hh"
#include "ibuf.hh"

#include "../module/begin.hh"


namespace tau::utf9
{

struct sym
{
  uint64_t h;
  sym(std::string const &s) : h(std::hash<std::string>{}(s)) {}
  sym(uint64_t h_)          : h(h_)                          {}
  operator uint64_t() const { return h; }
};

struct hash
{
  uint64_t h;
  hash(uint64_t h_) : h(h_) {}
  operator uint64_t() const { return h; }
  int compare(hash const x) const { return h > x.h ? 1 : h < x.h ? -1 : 0; }
};

struct pidfd
{
  uint32_t pid;
  uint32_t fd;

  pidfd(uint32_t pid_, uint32_t fd_) : pid(pid_), fd(fd_) {}

  operator uint64_t() const { return (uint64_t) pid << 32 | fd; }

  bool operator<(pidfd const &v) const { return pid < v.pid || pid == v.pid && fd < v.fd; }
  bool operator>(pidfd const &v) const { return pid > v.pid || pid == v.pid && fd > v.fd; }
};

struct greek
{
  enum letter : uint8_t { Α, Ι, Κ, Ρ, Θ, Τ, Ω };
  letter   l;
  uint32_t v;

  greek(letter l_) : l(l_), v(0) {}
  greek(letter l_, uint32_t v_) : l(l_), v(v_) {}
  greek(ibuf const &b, uint64_t i)
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


static_assert(sizeof(sym)   == sizeof(uint64_t));
static_assert(sizeof(hash)  == sizeof(uint64_t));
static_assert(sizeof(pidfd) == sizeof(uint64_t));
static_assert(sizeof(greek) == sizeof(uint64_t));

}


#include "../module/end.hh"

#endif
