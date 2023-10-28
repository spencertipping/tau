#ifndef τcptr_h
#define τcptr_h

#include "debug.hh"
#include "ctypes.hh"

#include "begin.hh"

namespace τ
{


// A simple, non-atomic shared and weak ptr that, because they aren't atomic
// (and hence, volatile), are faster than weak_ptr


Tt struct ptr_ctrl final
{
  iN          sr = 0;        // strong ref count
  iN          wr = 0;        // weak ref count
  T          *p  = nullptr;  // pointer to object
  F<void(T*)> df = nullptr;  // deleter function, or none for default
  bool        re = false;    // re-entrancy flag for post_release()

  void weak_acquire()   noexcept { ++wr; }
  void strong_acquire() noexcept { ++sr; }

  void weak_release()   noexcept { A(--wr >= 0, "ptr_ctrl wr < 0: " << wr); post_release(); }
  void strong_release() noexcept { A(--sr >= 0, "ptr_ctrl sr < 0: " << sr); post_release(); }

private:
  void post_release() noexcept
    { if (re) return;
      re = true;
      if (!sr && p) { let q = p; p = nullptr; df(q); df = nullptr; }
      if (!sr && !wr) { delete this; return; }
      re = false; }
};


Tt struct shared_ptr final
{
  T           *p;
  ptr_ctrl<T> *c;

  explicit shared_ptr(T *p_)          noexcept : p(p_), c(p ? new ptr_ctrl<T>{1, 0, p_, std::default_delete<T>()} : nullptr) {}
  shared_ptr(T *p_, F<void(T*)> &&f_) noexcept : p(p_), c(p ? new ptr_ctrl<T>{1, 0, p_, mo(f_)} : nullptr) {}

  shared_ptr()                        noexcept : p(nullptr), c(nullptr) {}
  shared_ptr(std::nullptr_t p_)       noexcept : p(nullptr), c(nullptr) {}
  shared_ptr(T *p_, ptr_ctrl<T> *c_)  noexcept : p(p_),  c(p ? c_  : nullptr) { if (c) c->strong_acquire(); }
  shared_ptr(shared_ptr<T> const &x)  noexcept : p(x.p), c(p ? x.c : nullptr) { if (c) c->strong_acquire(); }

  ~shared_ptr() { if (c) c->strong_release(); }


  shared_ptr<T> &operator=(shared_ptr<T> const &x) noexcept
    { if (x.c) x.c->strong_acquire();  // important: this must happen first
      if (c)   c  ->strong_release();
      p = x.p;
      c = x.c;
      return *this; }

  void reset()                       noexcept { *this = {nullptr, nullptr}; }
  void reset(shared_ptr<T> const &x) noexcept { *this = x; }
  void reset(T *x)                   noexcept
    { if (c) c->strong_release();
      p = x;
      c = new ptr_ctrl<T>{1, 0, p, std::default_delete<T>()}; }


  explicit operator bool() const noexcept { return p; }

  bool operator==(shared_ptr<T> const &x) const noexcept { return p == x.p; }

  T &operator* () const noexcept { A(p, "dereference empty shared_ptr"); return *p; }
  T *operator->() const noexcept { A(p, "dereference empty shared ptr"); return  p; }
  T *get()        const noexcept { return p; }

  iN use_count()  const noexcept { return c ? c->sr : 0; }

  template<class U>
  shared_ptr<U> as() const noexcept
    { return shared_ptr<U>{dynamic_cast<U*>(p), Rc<ptr_ctrl<U>*>(c)}; }
};


Tt struct weak_ptr final
{
  T           *p;
  ptr_ctrl<T> *c;

  weak_ptr()                       noexcept : p(nullptr), c(nullptr) {}
  weak_ptr(shared_ptr<T> const &s) noexcept : p(s.p), c(s.c) { if (c) c->weak_acquire(); }
  weak_ptr(weak_ptr<T>   const &w) noexcept : p(w.p), c(w.c) { if (c) c->weak_acquire(); }

  ~weak_ptr() { if (c) c->weak_release(); }


  weak_ptr<T> &operator=(weak_ptr<T> const &x) noexcept
    { if (x.c) x.c->weak_acquire();
      if (c)   c  ->weak_release();
      p = x.p;
      c = x.c;
      return *this; }

  void reset() noexcept
    { if (c) c->weak_release();
      p = nullptr;
      c = nullptr; }


  explicit operator bool() const noexcept { return !expired(); }

  bool expired() const noexcept { return !c || !c->p; }
  T   *get()     const noexcept { return p; }

  shared_ptr<T> lock() const noexcept
    { if (expired()) return {nullptr};
      else           return {c->p, c}; }

  iN use_count() const noexcept { return c ? c->sr : 0; }
};


}


Tt struct std::hash<τ::shared_ptr<T>>
{
  size_t operator()(τ::shared_ptr<T> const &x) const
    { return std::hash<void*>{}(x.p); }
};

Tt struct std::hash<τ::weak_ptr<T>>
{
  size_t operator()(τ::weak_ptr<T> const &x) const
    { return std::hash<void*>{}(x.p); }
};


#include "end.hh"

#endif
