#ifndef σwasm_gl_h
#define σwasm_gl_h

#include <emscripten.h>
#include <emscripten/html5.h>

#include "../gl.hh"
#include "../gl-text.hh"
#include "../begin.hh"

namespace σ
{


struct canvas_gl_window final : public virtual gl_window_base,
                                public virtual gl_usable
{
  EMSCRIPTEN_WEBGL_CONTEXT_HANDLE ctx;

  canvas_gl_window(Stc &id) : gl_window_base(id), gl_usable()
    { EmscriptenWebGLContextAttributes attrs;
      emscripten_webgl_init_context_attributes(&attrs);
      ctx = emscripten_webgl_create_context(id.c_str(), &attrs); }

  void use()  override { emscripten_webgl_make_context_current(ctx); }
  void swap() override { emscripten_webgl_commit_frame(); }
  vec2 dims() override
    { int w, h;
      emscripten_webgl_get_drawing_buffer_size(ctx, &w, &h);
      return {f64(w), f64(h)}; }
};


}

#include "../end.hh"

#endif
