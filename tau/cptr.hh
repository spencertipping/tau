#ifndef τcptr_h
#define τcptr_h

#include "debug.hh"
#include "ctypes.hh"

#include "begin.hh"

namespace τ
{


// A simple, non-atomic shared and weak ptr that, because they aren't atomic
// (and hence, volatile), are faster than weak_ptr


template<class T>
struct ptr_ctrl final
{
  iN          sr = 0;        // strong ref count
  iN          wr = 0;        // weak ref count
  T          *p  = nullptr;  // pointer to object
  F<void(T*)> df = nullptr;  // deleter function, or none for default
  bool        re = false;    // re-entrancy flag for post_release()

  void weak_acquire()   { ++wr; }
  void strong_acquire() { ++sr; }

  void weak_release()   { A(--wr >= 0, "ptr_ctrl wr < 0: " << wr); post_release(); }
  void strong_release() { A(--sr >= 0, "ptr_ctrl sr < 0: " << sr); post_release(); }

private:
  void post_release()
    { if (re) return;
      re = true;
      if (!sr && p) { let q = p; p = nullptr; df(q); df = nullptr; }
      if (!sr && !wr) { delete this; return; }
      re = false; }
};


template<class T>
struct shared_ptr final
{
  T           *p;
  ptr_ctrl<T> *c;

  explicit shared_ptr(T *p_)          : p(p_), c(p ? new ptr_ctrl<T>{1, 0, p_, std::default_delete<T>()} : nullptr) {}
  shared_ptr(T *p_, F<void(T*)> &&f_) : p(p_), c(p ? new ptr_ctrl<T>{1, 0, p_, std::move(f_)} : nullptr) {}

  shared_ptr()                        : p(nullptr), c(nullptr) {}
  shared_ptr(std::nullptr_t p_)       : p(nullptr), c(nullptr) {}
  shared_ptr(T *p_, ptr_ctrl<T> *c_)  : p(p_),  c(p ? c_  : nullptr) { if (c) c->strong_acquire(); }
  shared_ptr(shared_ptr<T> const &x)  : p(x.p), c(p ? x.c : nullptr) { if (c) c->strong_acquire(); }

  ~shared_ptr() { if (c) c->strong_release(); }


  shared_ptr<T> &operator=(shared_ptr<T> const &x)
    { if (x.c) x.c->strong_acquire();  // important: this must happen first
      if (c)   c  ->strong_release();
      p = x.p;
      c = x.c;
      return *this; }

  void reset() { *this = {nullptr, nullptr}; }
  void reset(shared_ptr<T> const &x) { *this = x; }
  void reset(T *x)
    { if (c) c->strong_release();
      p = x;
      c = new ptr_ctrl<T>{1, 0, p, std::default_delete<T>()}; }


  operator bool() const { return p; }

  T &operator* () const { A(p, "dereference empty shared_ptr"); return *p; }
  T *operator->() const { A(p, "dereference empty shared ptr"); return  p; }
  T *get()        const { return p; }

  iN use_count()  const { return c ? c->sr : 0; }

  template<class U>
  shared_ptr<U> as() const
    { return shared_ptr<U>{dynamic_cast<U*>(p), Rc<ptr_ctrl<U>*>(c)}; }
};


template<class T>
struct weak_ptr final
{
  T           *p;
  ptr_ctrl<T> *c;

  weak_ptr()                       : p(nullptr), c(nullptr) {}
  weak_ptr(shared_ptr<T> const &s) : p(s.p), c(s.c) { if (c) c->weak_acquire(); }
  weak_ptr(weak_ptr<T>   const &w) : p(w.p), c(w.c) { if (c) c->weak_acquire(); }

  ~weak_ptr() { if (c) c->weak_release(); }


  weak_ptr<T> &operator=(weak_ptr<T> const &x)
    { if (x.c) x.c->weak_acquire();
      if (c)   c  ->weak_release();
      p = x.p;
      c = x.c;
      return *this; }

  void reset()
    { if (c) c->weak_release();
      p = nullptr;
      c = nullptr; }


  operator bool() const { return !expired(); }

  bool expired()  const { return !c || !c->p; }
  T   *get()      const { return p; }

  shared_ptr<T> lock() const
    { if (expired()) return {nullptr};
      else           return {c->p, c}; }

  iN use_count() const { return c ? c->sr : 0; }
};


}


template<class T> struct std::hash<τ::shared_ptr<T>>
{
  size_t operator()(τ::shared_ptr<T> const &x) const
    { return std::hash<void*>{}(x.p); }
};

template<class T> struct std::hash<τ::weak_ptr<T>>
{
  size_t operator()(τ::weak_ptr<T> const &x) const
    { return std::hash<void*>{}(x.p); }
};


#include "end.hh"

#endif
