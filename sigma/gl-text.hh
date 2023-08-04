#ifndef σgl_text_h
#define σgl_text_h

#include "gl-geom.hh"
#include "gl-prims.hh"
#include "begin.hh"

namespace σ
{


struct gl_text final : public virtual gl_usable
{
  St     t;  // text
  St     f;  // font
  color  bg   = {0,0,0,0};
  color  fg   = {1,1,1,1};
  GLuint tid  = 0;
  vec2   dims = {0, 0};

  gl_text()               = default;
  gl_text(gl_text const&) = default;
  gl_text(gl_text &&t_)
    : t(mo(t_.t)), f(mo(t_.f)), bg(t_.bg), fg(t_.fg), tid(t_.tid), dims(t_.dims)
    { t_.tid = 0; }

  gl_text(Stc &t, Stc &f, colorc &bg, colorc &fg)
    : t(t), f(f), bg(bg), fg(fg), tid(0), dims(0, 0) {}

  gl_text(ηic &x) : gl_text(x.cs(), x[1].cs(), x[2].η(), x[3].η()) {}

  ~gl_text() { clear(); }

  gl_text &operator=(gl_text const&) = default;
  gl_text &operator=(gl_text&&)      = default;


  // NOTE: rendering internals are platform-specific
  GLuint texture();
  vec2   measure();

  void clear()
    { if (tid) glDeleteTextures(1, &tid);
      tid  = 0;
      dims = {0, 0}; }

  uN tsize() const { return tid ? dims.x * dims.y * 4 : 0; }

  void use() override
    { if (!tid) tid = texture();
      glActiveTexture(GL_TEXTURE0);
      glBindTexture(GL_TEXTURE_2D, tid); }
};


}

#include "end.hh"

#endif
