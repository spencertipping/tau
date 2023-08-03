#ifndef σgl_text_h
#define σgl_text_h

#include <xxhash.h>

#include "gl-geom.hh"
#include "begin.hh"

namespace σ
{

typedef P<u64, u64> gl_text_key;

}


namespace std
{

Tn struct hash<σ::gl_text_key>
{
  τ::uN operator()(σ::gl_text_key const &k) const
    { return ~k.first ^ k.second; }
};

}


namespace σ
{


struct gl_text final
{
  St     f;  // font
  St     t;  // text
  color  bg   = {0,0,0,0};
  color  fg   = {1,1,1,1};
  GLuint tid  = 0;
  vec2   dims = {0, 0};

  gl_text()               = default;
  gl_text(gl_text const&) = default;
  gl_text(gl_text &&t_)
    : f(mo(t_.f)), t(mo(t_.t)), bg(t_.bg), fg(t_.fg), tid(t_.tid), dims(t_.dims)
    { t_.tid = 0; }

  gl_text(Stc &f, Stc &t, colorc &bg, colorc &fg)
    : f(f), t(t), bg(bg), fg(fg), tid(0), dims(0, 0) {}

  ~gl_text() { clear(); }

  // NOTE: rendering internals are platform-specific
  GLuint texture();
  vec2   measure();

  gl_text_key key() const
  // FIXME: must incorporate colors
    { return {XXH64(f.data(), f.size(), 0), XXH64(t.data(), t.size(), 0)}; }

  void clear()
    { if (tid) glDeleteTextures(1, &tid);
      tid  = 0;
      dims = {0, 0}; }

  uN tsize() const { return tid ? dims.x * dims.y * 4 : 0; }
};


}

#include "end.hh"

#endif
