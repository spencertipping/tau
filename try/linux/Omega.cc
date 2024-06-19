#include <cmath>
#include <iostream>
#include <random>

#include "../../tau.hh"
#include "../../sigma.hh"

#include "../../tau/begin.hh"


using namespace τ;
using namespace σ;


void try_pwrite_and_size()
{
  unlink("/tmp/pwrite-and-size");
  Ωfd f = Ωopen("/tmp/pwrite-and-size", true);
  f->pwrite("hello ", 6, 0);
  f->pwrite("world!", 6, 6);
  A(f->size() == 12, "f->size() == " << f->size() << ", expected 12");
  std::cout << "pwrite/size ok" << std::endl;
}


void try_Ωa()
{
  unlink("/tmp/omegaa-test.kv");
  unlink("/tmp/omegaa-test.hm");
  Ωa a("/tmp/omegaa-test", true);

  A(a.get(ηm{} << "foo").empty(), "nonempty fetch foo");
  a.add(ηm{} << "foo", ηm{} << "bar");
  AE(a.get(ηm{} << "foo").y().next().cs(), "bar");

  a.commit();
  AE(a.get(ηm{} << "foo").y().next().cs(), "bar");
  a.commit(true);
  AE(a.get(ηm{} << "foo").y().next().cs(), "bar");

  std::cout << "Ωa ok" << std::endl;
}


void try_Ωa_stress()
{
  unlink("/tmp/omegaa-stresstest.kv");
  unlink("/tmp/omegaa-stresstest.hm");
  Ωa aw ("/tmp/omegaa-stresstest", true);
  Ωa ar1("/tmp/omegaa-stresstest", false);
  Ωa ar2("/tmp/omegaa-stresstest", false);

  Stc b64("0123456789abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ+/");

  i64 ntests = 0;
  for (i64 i = 0; i < 100000; ++i)
  {
    aw.add(ηm{} << "sk" << i, ηm{} << "sv" << i << b64.substr(0, i * 25519 % (b64.size() + 1)));
    if ((i + 1) % 3094 == 0)
    {
      aw.commit();
      for (i64 j = std::max<i64>(0, i * 1337 % i + (i - 2000)); j < i; ++j)
      {
        let k = ηm{} << "sk" << j;
        let awv = aw.get(k);
        A(!awv.empty(), "empty key " << k << ", i = " << i << ", j = " << j << ", kh = " << std::hex << k.y().hash());
        AE(awv.y()[0].η(), k.y());
        AE(awv.y()[1].cs(), "sv");
        AE(awv.y()[2].ci(), j);
        AE(awv.y()[3].t(), ηtype::string);
        AE(ar1.get(ηm{} << "sk" << j), awv);
        AE(ar2.get(ηm{} << "sk" << j), awv);
        ++ntests;
      }
    }
  }

  std::cout << "Ωa stress ok; #tests = " << ntests << std::endl;
}


void Ωa_bench(i64 iterations = 1048576)
{
  unlink("/tmp/omegaa-bench.kv");
  unlink("/tmp/omegaa-bench.hm");
  Ωa a("/tmp/omegaa-bench", true);

  {
    let m = measurement_for("Ωa_bench/add");
    for (i64 i = 0; i < iterations; ++i)
    {
      let k = ηm{} << "key" << i;
      let v = ηm{} << "val" << i;
      {
        let t = m->start();
        a.add(k, v);
      }
      if (i % 65536 == 0) std::cerr << "\radd " << i << " / " << iterations << "  " << std::flush;
    }
    a.commit(true);
  }

  V<i64> xs;
  xs.reserve(iterations);
  for (i64 i = 0; i < iterations; ++i) xs.push_back(i);
  std::shuffle(xs.begin(), xs.end(), std::mt19937_64{});

  {
    let m = measurement_for("Ωa_bench/fetch");
    for (i64 i = 0; i < i64(xs.size()); ++i)
    {
      let k = ηm{} << "key" << xs[i];
      ηm r;
      {
        let t = m->start();
        r = a.get(k);
      }
      AE(r, ηm{} << k << "val" << xs[i]);
      if (i % 65536 == 0) std::cerr << "\rfetch " << i << " / " << iterations << "  " << std::flush;
    }
  }

  std::cerr << measurement_for("Ωa_bench/add") << std::endl;
  std::cerr << measurement_for("Ωa_bench/fetch") << std::endl;
  std::cout << "Ωa_bench ok" << std::endl;
}


void try_Ωl()
{
  unlink("/tmp/omegal-test");
  { Ωl l1("/tmp/omegal-test", true); }  // write header
  Ωl l2("/tmp/omegal-test", false);

  Ωl::key k1, k2, k3, k4;
  ηm v21, v22, v23, v24;
  {
    Ωl l1("/tmp/omegal-test", true);
    k1 = l1 << (ηm{} << "hi there" << 5);  v21 = l2[k1];
    k2 = l1 << (ηm{});                     v22 = l2[k2];

    A(k1 == 16, "k1 = " << k1);
    A(k2 == k1 + v21.lsize() + 4, "k2 = " << k2);
  }

  Ωl l1("/tmp/omegal-test", true);
  k3 = l1 << (ηm{} << true);         v23 = l2[k3];
  k4 = l1 << (ηm{} << ηm{} << ηm{}); v24 = l2[k4];
  A(k3 == k2 + v22.lsize() + 4, "k3 = " << k3);
  A(k4 == k3 + v23.lsize() + 4, "k4 = " << k4);

  A(l1[k1] == v21, "l1[k1] v21");
  A(l1[k2] == v22, "l1[k2] v22");
  A(l1[k3] == v23, "l1[k3] v23");
  A(l1[k4] == v24, "l1[k4] v24");

  ηmc *ms[4] = {&v21, &v22, &v23, &v24};
  int  i = 0;
  for (let &x : l2) A(x == *ms[i++], "array mismatch at i = " << i - 1);

  std::cout << "Ωl ok" << std::endl;
}


int main()
{
  τassert_begin

  try_pwrite_and_size();
  try_Ωl();
  try_Ωa();
  try_Ωa_stress();
  Ωa_bench(1048576);
  Ωa_bench(1048576 * 8);

  return 0;
  τassert_end
}
