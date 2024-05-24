#include "../../tau.hh"
#include "../../sigma.hh"
#include "../../tau/begin.hh"

using namespace τ;
using namespace σ;


void try_lmdb()
{
  τe   t;
  lmdb l(t, "/tmp/test.db", "t");

  uN n_commits = 0;
  uN n_repacks = 0;

  for (i64 i = 0;; ++i)
  {
    l.set(ηm{} << i, ηm{} << i);

    let nc = l.prof_commit_outer()->n();
    let nr = l.prof_repack_outer()->n();

    if (nc != n_commits || nr != n_repacks)
    {
      n_commits = nc;
      n_repacks = nr;
      std::cout << "i = " << i << ", commits = " << n_commits
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
