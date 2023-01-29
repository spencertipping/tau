#include "arch.hh"
#if τhas_emscripten_fiber

#include "tau-emscripten.hh"
#include "begin.hh"

namespace τ
{


τe *τg = nullptr;


void τset(τe *t)
{
  τg = t;
}


void τstep(void *t_)
{
  τe &t = *Rc<τe*>(t_);
  if (t.should_step()) t();
}


τe &τe::wake()
{
  while (now() >= hn()) l.r(h.top().l), h.pop();
  return *this;
}


τe &τe::schedule()
{
  let t = now();
  if (hn() < forever())
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
  l.go();
  return schedule();
}


}

#include "end.hh"
#endif
