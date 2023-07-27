#ifndef τmptr_h
#define τmptr_h

#include <memory>

#include "ctypes.hh"
#include "begin.hh"

namespace τ
{


// mptr: a mutable managed pointer. This is useful when you have a C API
// that takes addresses to pointers, which isn't compatible with shared_ptr.
Tt struct mptr final
{
  mptr(       F<void(T*)> df_ = std::free) : p(nullptr), df(df_) {}
  mptr(T* p_, F<void(T*)> df_ = std::free) : p(p_),      df(df_) {}

  ~mptr() { if (p) df(p); }

  operator bool() const { return p; }

  mptr &operator=(T *x)  { if (p) df(p); p = x; return *this; }
  T**   operator&()      { free(); return &p; }
  operator    T*() const { return p; }
  T  &operator->() const { return *p; }

  Tx X *as() const { return Rc<X*>(p); }

  T *get() const { return as<T>(); }

  T  *disown() { let r = p;    p = nullptr; return r; }
  mptr &free() { if (p) df(p); p = nullptr; return *this; }


protected:
  T*          p;
  F<void(T*)> df;
};


}

#include "end.hh"

#endif
