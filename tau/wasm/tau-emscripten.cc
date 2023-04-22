#include "tau-emscripten.hh"
#include "../begin.hh"

namespace τ
{


static τe *τg = nullptr;


void τset(τe *t)
{
  if (t) A(!τg, "τset multiple; old is " << Rc<void*>(τg));
  τg = t;
}


void τstep(void *t_)
{
  τe &t = *Rc<τe*>(t_);
  if (t.should_step()) t();
}


τe &τe::wake()
{
  while (now() >= hn()) l_.r(h_.top().l, λs::R), h_.pop();
  return *this;
}


τe::operator bool() const
{
  return l_() || nts || hn() != forever();
}


τe &τe::go(F<bool(τe&)> &&f)
{
  go_f = mo(f);
  while (l_())
  {
    wake();
    l_.step();
  }
  return schedule();
}


bool τe::should_step()
{
  return go_f(*this);
}


τe &τe::schedule()
{
  let t = now();
  if (l_() || hn() < forever())
    if (t < hn())
    { let dt = (hn() - t) / 1ms;
      emscripten_async_call(τstep, this, std::min(dt, Sc<decltype(dt)>(Nl<int>::max()))); }
    else
      emscripten_async_call(τstep, this, 0);
  return *this;
}


τe &τe::operator()()
{
  wake();
  l_.step();
  return schedule();
}


}

#include "../end.hh"
