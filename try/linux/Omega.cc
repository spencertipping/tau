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


int main()
{
  τassert_begin

  try_pwrite_and_size();

  unlink("/tmp/omegaf-test");
  { Ωl l1("/tmp/omegaf-test", true); }  // write header
  Ωl l2("/tmp/omegaf-test", false);

  Ωl::key k1, k2, k3, k4;
  ηm v21, v22, v23, v24;
  {
    Ωl l1("/tmp/omegaf-test", true);
    k1 = l1 << (ηm{} << "hi there" << 5);  v21 = l2[k1];
    k2 = l1 << (ηm{});                     v22 = l2[k2];

    A(k1 == 16, "k1 = " << k1);
    A(k2 == k1 + v21.lsize() + 4, "k2 = " << k2);
  }

  Ωl l1("/tmp/omegaf-test", true);
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
  return 0;
  τassert_end
}
