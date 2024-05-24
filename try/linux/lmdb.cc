#include "../../tau.hh"
#include "../../sigma.hh"
#include "../../tau/begin.hh"

using namespace τ;
using namespace σ;


void lmdb_failure_case()
{
  MDB_env *e;
  mdb_env_create(&e);
  mdb_env_set_mapsize(e, 1ull << 40);
  mdb_env_set_maxdbs(e, 64);
  mdb_env_open(e, "/tmp/mdbfail.db",
               MDB_NOSYNC | MDB_NOSUBDIR | MDB_NORDAHEAD | MDB_NOTLS, 0664);

  MDB_txn *w;

}


void try_lmdb()
{
  τe   t;
  lmdb l(t, "/tmp/test.db", "t",
         1ull << 40,
         64,
         8ull << 10,
         2.0);

  uN n_commits = 0;
  uN n_blocks  = 0;
  uN n_repacks = 0;

  for (i64 i = 0;; ++i)
  {
    if (i % 100 == 0) std::cout << "pre-set i = " << i << std::endl;
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

      MDB_txn *r;
      mdb_txn_begin(l.env().get(), nullptr, MDB_RDONLY, &r);

      MDB_cursor *c;
      mdb_cursor_open(r, l.dbi(), &c);

      S<i64> ints;
      MDB_val k, v;
      while (mdb_cursor_get(c, &k, &v, MDB_NEXT) == 0)
      {
        let ki = ηi{Rc<u8c*>(k.mv_data), k.mv_size};
        let vi = ηi{Rc<u8c*>(v.mv_data), v.mv_size};
        ints.insert(ki.as<i64>());
        if (vi.empty() || vi.as<i64>() != ki.as<i64>())
          std::cout << "cursor-loop mismatch: " << ki.as<i64>() << " -> " << vi.as<i64>() << std::endl;
      }
      mdb_cursor_close(c);
      mdb_txn_abort(r);

      if (ints.size() != i + 1)
      {
        std::cout << "cursor-loop size mismatch: " << ints.size() << " vs " << i << std::endl;
        for (int j = 0; j <= i; ++j)
          if (!ints.contains(j))
            std::cout << "cursor-loop missing: " << j << std::endl;
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
