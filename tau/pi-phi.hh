#ifndef τπφ_h
#define τπφ_h

#include "phi.hh"
#include "phi-fn.hh"
#include "phi-str.hh"
#include "phi-ctor.hh"

#include "pi-int.hh"

#include "begin.hh"

namespace τ
{


template<class T>
φ<πf<T>> φlift(φ<T> p)
{
  // NOTE: we must explicitly specify the lambda type; otherwise C++
  // fails to match it to φm and complains. This seems to me like a C++
  // defect, but there's probably something about function return-type
  // erasure or some such that I don't know about yet.
  F<πf<T>(T)> f = [](T r) { return [r](πi) { return r; }; };
  return φm(p, f);
}


φ<i64>   φint_literal();
φ<f64>   φfloat_literal();
φ<St>    φstr_literal();
φ<η1sig> φsig_literal();

φ<uN>    φηtuple_key();
φ<St>    φηmap_key();


φ<πf<i64>>   φint();
φ<πf<f64>>   φfloat();
φ<πf<St>>    φstr();
φ<πf<η1sig>> φsig();


// TODO: named variables


}

#include "end.hh"


#endif
