#pragma once


#include <cstdint>
#include <string>


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

}
