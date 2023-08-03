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

  virtual void use()  = 0;
  virtual void swap() = 0;
  virtual vec2 dims() = 0;
};


// Each frame is rendered by streaming η render instructions into this struct.
// It holds the current render state and allows for FBO allocations.
//
// NOTE: this struct outlives a frame. This allows resources to be cached.
// All GL-allocated resources and textures are managed automatically using
// frame-level GC.
struct gl_render_state final
{
  gl_render_state(gl_window_base *w_) : w(w_), f(false) {}
  ~gl_render_state();

  gl_render_state &begin();             // start a frame
  gl_render_state &end();               // end a frame
  gl_render_state &operator<<(ηic &x);  // append to frame, or one-off

  void run();  // render a frame with stored instructions

  gl_text        &text(Stc &f, Stc &t, colorc &bg, colorc &fg);
  gl_vbo         &vbo (Stc &n, ηic &xs);
  gl_fbo_texture &fbo (Stc &n, ηic &r);


protected:
  gl_window_base *w;   // target window
  ηm              rs;  // base render operations
  bool            f;   // are we inside a frame

  M<gl_text_key, gl_text> ts;
  M<St, gl_vbo>           vs;
  M<St, gl_fbo_texture>   fs;

  S<gl_text_key>          mts;  // GC-marked texts
  S<St>                   mvs;
  S<St>                   mfs;

  void mark_t(gl_text_key const &k) { if (f) mts.insert(k); }
  void mark_v(Stc &k)               { if (f) mvs.insert(k); }
  void mark_f(Stc &k)               { if (f) mfs.insert(k); }

  void gc_begin();     // begin marking new objects
  void gc_end();       // collect all unmarked objects

  void apply(ηic &x);  // apply a render op to this state
};


}

#include "end.hh"

#endif
