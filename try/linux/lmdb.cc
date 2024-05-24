#include "../../tau.hh"
#include "../../sigma.hh"
#include "../../tau/begin.hh"

using namespace τ;
using namespace σ;


void try_lmdb()
{
  τe   t;
  lmdb l(t, "/tmp/test.db", "t",
         65536,   // 64KB initial mapsize
         64,      // 64 DBs
         524288,  // 512KB commit size
         1.1);    // repack every 1.1x disk expansion

  uN n_commits = 0;
  uN n_blocks  = 0;
  uN n_repacks = 0;

  for (i64 i = 0;; ++i)
  {
    l.set(ηm{} << i, ηm{} << i);

    let nc = l.prof_commit_write()->n();
    let nb = l.prof_commit_block()->n();
    let nr = l.prof_repack_inner()->n();

    if (nc != n_commits || nb != n_blocks || nr != n_repacks)
    {
      n_commits = nc;
      n_blocks  = nb;
      n_repacks = nr;
      std::cout << "i = " << i
                << ", commits = " << n_commits
                << ", blocks = "  << n_blocks
                << ", repacks = " << n_repacks << std::endl;

      for (i64 j = 0; j <= i; ++j)
      {
        let x = l.get(ηm{} << j);
        if (x.x.empty())
          std::cout << "i = " << i << ", j = " << j << ", empty" << std::endl;
        else if (x.x.as<i64>() != j)
          std::cout << "i = " << i << ", j = " << j
                    << ", mismatch: " << l.get(ηm{} << j).x.as<i64>() << std::endl;
      }
    }
  }
}


int main()
{
  try_lmdb();
  return 0;
}

#include "../../tau/end.hh"
