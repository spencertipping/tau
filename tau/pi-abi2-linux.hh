#ifndef τπabi2_linux
#define τπabi2_linux

#include <errno.h>
#include <signal.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>

#include <cstring>
#include <memory>

#include <zstd.h>

#include "pi-abi1.hh"
#include "linux/io.hh"

#include "pi-abi-begin.hh"

namespace τ
{


void πabi2_linux_fileio(πabi &a)
{
  a .def("F?", I{
      struct stat s;
      let p = i.dpop().c_str();
      let r = stat(p, &s);
      free(p);
      if (r) i.dpush(errno);
      else   i.dpush(
        o9t("dev"_y,     s.st_dev,
            "ino"_y,     s.st_ino,
            "mode"_y,    s.st_mode,
            "nlink"_y,   s.st_nlink,
            "uid"_y,     s.st_uid,
            "gid"_y,     s.st_gid,
            "dev"_y,     s.st_dev,
            "size"_y,    s.st_size,
            "blksize"_y, s.st_blksize,
            "blocks"_y,  s.st_blocks,
            "atim"_y,    Sc<u64>(s.st_atim.tv_sec * 1000000000ull + s.st_atim.tv_nsec),
            "mtim"_y,    Sc<u64>(s.st_mtim.tv_sec * 1000000000ull + s.st_mtim.tv_nsec),
            "ctim"_y,    Sc<u64>(s.st_ctim.tv_sec * 1000000000ull + s.st_ctim.tv_nsec)).m());
    });
}


void πabi2_linux_general(πabi &a)
{
  a .def("E?", I{ i.dpush(strerror(Sc<int>(i.dpop()))); });
}


void πabi2_linux_process(πabi &a)
{
  a .def("X:", I{ i.dpush(fork()); })
    .def("X?", I{ TODO("waitpid"); })
    .def("X_", I{
        let s = Sc<int>(i.dpop());
        i.dpush(kill(Sc<int>(i.dpop()), s)); })
    .def("X=", I{
        let c = i.dpop().c_str();
        let a = i.dpop();
        let e = i.dpop();
        ch **as = new ch*[a.as_tuple().len()];
        ch **es = new ch*[e.as_tuple().len()];
        uN j = 0; for (let x : a) as[j++] = x.c_str();
           j = 0; for (let x : e) es[j++] = x.c_str();
        let r = execvpe(c, as, es);
        for (j = 0; j < a.as_tuple().len(); ++j) free(as[j]);
        for (j = 0; j < e.as_tuple().len(); ++j) free(es[j]);
        free(c);
        i.dpush(r); })
    .def("X/=", I{
        let c = i.dpop().c_str();
        let a = i.dpop();
        ch **as = new ch*[a.as_tuple().len()];
        uN j = 0; for (let x : a) as[j++] = x.c_str();
        let r = execvp(c, as);
        for (j = 0; j < a.as_tuple().len(); ++j) free(as[j]);
        free(c);
        i.dpush(r); });
}


void πabi2_linux_φ(πabi &a)
{
  a .def("F>", I{
        Φf<o9fdr> g{i.f, Sc<fd_t>(i.dpop())};
        TODO("FIXME: F> fails with resource unavailable");
        i.dpush(g.o); })
    .def("F<", I{
        Φf<o9fdr> g{i.f, Sc<fd_t>(i.dpop())};
        i.dpush(i.dpop() >> g); });
}


void πabi2_linux(πabi &a)
{
  πabi2_linux_fileio(a);
  πabi2_linux_general(a);
  πabi2_linux_process(a);
  πabi2_linux_φ(a);
}


πabi const &πabi2_linux()
{
  static πabi a;
  static bool i = false;
  if (!i) πabi1(a), πabi2_linux(a);
  i = true;
  return a;
}


}

#include "pi-abi-end.hh"


#endif
