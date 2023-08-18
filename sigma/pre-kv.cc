#include "pre-kv.hh"
#include "../tau/begin.hh"

namespace σ::pre
{


struct kv_native_ final : public virtual kv_
{
  kv_native_() : kv_() {}

  ηi   get(ηic &k)         override { return m[k].y(); }
  void set(ηic &k, ηic &v) override { m[k] = v; }
  void del(ηic &k)         override { m.erase(k); }
  bool has(ηic &k)         override { return m.contains(k); }

protected:
  M<ηm, ηm> m;
};


Sp<kv_> kv_native()
{
  return Sp<kv_>{new kv_native_};
}


}

#include "../tau/end.hh"
