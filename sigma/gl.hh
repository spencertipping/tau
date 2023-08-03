#ifndef σgl_h
#define σgl_h

#include <GLES2/gl2.h>
#include <xxhash.h>

#include "gl-geom.hh"
#include "gl-text.hh"
#include "gl-prims.hh"
#include "begin.hh"

namespace σ
{

using namespace τ;


struct gl_window_base
{
  Stc addr;  // X11 display or canvas ID

  gl_window_base(Stc &addr_) : addr(addr_) {}

  virtual ~gl_window_base() {}

  virtual void swap() = 0;
  virtual vec2 dims() = 0;
};


// Render ops in (see gl_render_state), events out
Γ Γgl_window(Stc&, Ψd = Ψd::f);


// Each frame is rendered by streaming η render instructions into this struct.
// It holds the current render state and allows for FBO allocations.
//
// NOTE: this struct outlives a frame. This allows resources to be cached.
// All GL-allocated resources and textures are managed automatically using
// frame-level GC.
struct gl_render_state final
{
  gl_render_state(gl_usable *t_) : t(t_), f(false) {}
  ~gl_render_state() {}

  gl_render_state &begin();             // start a frame
  gl_render_state &end();               // end a frame
  gl_render_state &operator<<(ηic &x);  // append to frame, or one-off

  void run();  // render a frame with stored instructions

  gl_text        &text(ηic&);
  gl_vbo         &vbo (ηic&);
  gl_fbo_texture &fbo (ηic&);
  gl_program     &prog(ηic&);
  void            uniforms(ηic&);


protected:
  gl_usable *t;   // render target
  ηm         rs;  // base render operations
  bool       f;   // are we inside a frame

  M<h256, gl_text>        ts;
  M<h256, gl_vbo>         vs;
  M<h256, gl_fbo_texture> fs;
  M<h256, gl_program>     ps;

  S<h256>                 mts;  // GC-marked objects
  S<h256>                 mvs;
  S<h256>                 mfs;
  S<h256>                 mps;

  // NOTE: these mark functions receive just the key, not necessarily the whole
  // object -- for example, prog() doesn't need to have all shader sources, just
  // the program name.
  void mark_t(h256 const &x) { if (f) mts.insert(x); }
  void mark_v(h256 const &x) { if (f) mvs.insert(x); }
  void mark_f(h256 const &x) { if (f) mfs.insert(x); }
  void mark_p(h256 const &x) { if (f) mps.insert(x); }

  void clear_marks();

  void gc_begin();     // begin marking new objects
  void gc_end();       // collect all unmarked objects

  void apply(ηic &x);  // apply a render op to this state
};


}

#include "end.hh"

#endif
