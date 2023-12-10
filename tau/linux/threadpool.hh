#ifndef τlinux_threadpool_h
#define τlinux_threadpool_h

#include "../types.hh"

#include "../begin.hh"

namespace τ
{


struct thread_pool final
{
  thread_pool(uN n, F<void()> ef_) : rn(0), ef(ef_), fin(false)
    { for(uN i = 0; i < n; ++i)
        ws.emplace_back([this]
          { while (true)
            { F<void()> f;
              { Ul<Mu> l(this->qm);
                this->cv.wait(
                  l, [this] { return this->fin || !this->ts.empty(); });
                if (this->fin && this->ts.empty()) return;
                f = mo(this->ts.front());
                this->ts.pop(); }
              ++this->rn;
              f();
              --this->rn; } }); }

  ~thread_pool()
    { { Ul<Mu> l(qm); fin = true; }
      cv.notify_all();
      for (auto &w : ws) w.join(); }


  uN running() const { return rn; }
  uN queued()  const { return ts.size(); }
  uN tasks()   const { return rn + ts.size(); }


  template<class F, class... Xs>
  auto operator()(F &&f, Xs&&... xs) -> Fu<decltype(f(xs...))>
  {
    using T = decltype(f(xs...));
    let t = std::make_shared<Pt<T()>>(std::bind(std::forward<F>(f), std::forward<Xs>(xs)...));

    {
      Ul<Mu> l(qm);
      A(!fin, "enqueue on stopped thread_pool");
      ts.emplace([this, t](){ (*t)(); this->ef(); });
    }
    cv.notify_one();
    return t->get_future();
  }


protected:
  At<uN>       rn;  // number of running threads
  V<Th>        ws;
  Q<F<void()>> ts;
  Mu           qm;  // protects the queue
  Cv           cv;
  F<void()>    ef;  // call this after each promise is set
  bool         fin;
};


}

#include "../end.hh"


#endif
