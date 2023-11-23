#include <boost/context/stack_context.hpp>
#include <sys/mman.h>
#include <unistd.h>

#include "../types.hh"
#include "../lambda-types.hh"
#include "../lambda-class.hh"


#include "../begin.hh"

namespace τ
{

namespace λbc = boost::context;


struct guarded_stack_allocator
{
  static uS page_size()
    {
      static uS page_size_ = 0;
      if (!page_size_) page_size_ = sysconf(_SC_PAGESIZE);
      return page_size_;
    }

  bc::stack_context allocate()
    {
      // Allocate memory with an extra page at each end for the guard
      let m = ::mmap(nullptr, λss + 2 * page_size(),
                     PROT_READ | PROT_WRITE,
                     MAP_PRIVATE | MAP_ANON, -1, 0);

      ::mprotect(m, page_size(), PROT_NONE);
      ::mprotect(Sc<ch*>(m) + page_size() + λss, page_size(), PROT_NONE);

      bc::stack_context r;
      r.size = λss;
      r.sp   = Sc<ch*>(m) + page_size() + λss;
      return r;
    }

  void deallocate(boost::context::stack_context& sctx)
    {
      ::munmap(Sc<ch*>(sctx.sp) - sctx.size - page_size(),
               sctx.size + 2 * page_size());
    }
};


static guarded_stack_allocator λssa;


bool λmi;  // true if current continuation is main


void λm(λbc::continuation &&cc)
{
  if (cc) **λmk() = mo(cc);
}


λ::λ(λf &&f_)
  : f      (mo(f_)),
    k      (nullptr),
    is_done(false)
{
  λinit();
  if (λmi)
  {
    λtrack_alloc(λss);
    k = new λbc::continuation(λbc::callcc(
                                std::allocator_arg, λssa,
                                [&](λbc::continuation &&cc)
                                  { λm(cc.resume());
                                    f();
                                    fin();
                                    return mo(**λmk()); }));
  }
}


λ::~λ()
{
  if (k)
  {
    λtrack_free(λss);
    delete k;
  }
}


void λ::fin()
{
  A(!done(), "fin() called on done λ");
  A(!λmi,    "fin() called on main λ");
  is_done = true;
  if (k) { λtrack_free(λss); delete k; k = nullptr; }
}


λ &λ::operator()()
{
  A(!done(), "λ called after fin()");
  A(λmi,     "λ called not from main");

  λmi = false;
  if (!k)
  {
    // NOTE: I believe we need to capture λmk from the inner continuation's
    // frame of reference so it knows where to return. I haven't fully worked
    // this out yet, but cc.resume() at the beginning makes it work reliably.
    k = new λbc::continuation;
    λtrack_alloc(λss);
    auto cc = λbc::callcc(
      std::allocator_arg, λssa,
      [&](λbc::continuation &&cc)
        { λm(cc.resume());
          f();
          fin();
          return mo(**λmk()); });
    cc = cc.resume();
    if (k) *k = mo(cc);
  }
  else
  {
    auto cc = k->resume();
    if (k) *k = mo(cc);  // NOTE: k can be nulled within k->resume()
  }

  λmi = true;
  return *this;
}


void λy()
{
  A(!λmi,   "λy() from main");
  A(*λmk(), "λy() with no main continuation");
  **λmk() = (*λmk())->resume();
}


void λinit_()
{
  λmi = true;
  *λmk() = new λbc::continuation;
}


}

#include "../end.hh"
