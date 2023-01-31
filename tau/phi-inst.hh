#ifndef τφinst_h
#define τφinst_h


#include "types.hh"
#include "gamma.hh"
#include "Gamma.hh"


// Instantiation macros for φ templates: this allows us to precompile
// specific template instantiations for a variety of φ<T>s.
//
// φinst contains a single list of types for which we expect to have
// φ<T> be usable.

#define φinst(f) \
  f(St) \
  f(Sp<γ>) \
  f(Γ)


// This shouldn't be defined in end-user #include "tau.hh"
#define τnoexport 1


#endif
