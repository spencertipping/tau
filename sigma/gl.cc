#include <GLES2/gl2.h>
#include <stdexcept>

#include "gl.hh"
#include "begin.hh"

namespace σ
{


gl_render_state &gl_render_state::begin()
{
  f = true;
  rs.clear();
  gc_begin();
  return *this;
}


gl_render_state &gl_render_state::end()
{
  gc_end();
  f = false;
  return *this;
}


gl_render_state &gl_render_state::operator<<(ηic &x)
{
  if (f) rs << x;
  apply(x);
  return *this;
}


void gl_render_state::apply(ηic &x)
{
  let s = x[0].n();
  if (s == "text")
    ηhauto([this](Stc &t,
                  Stc &f,
                  T<f64, f64, f64, f64> bg,
                  T<f64, f64, f64, f64> fg)
      {
        return text(t, f, bg, fg);
      })(x);

}


void gl_render_state::run()
{
  for (let &x : rs.y()) apply(x.η());
}


gl_text &gl_render_state::text(Stc    &t,
                               Stc    &f,
                               colorc &bg,
                               colorc &fg)
{
  TODO("gl_render_state::text(Stc &t, Stc &f, colorc &bg, colorc &fg): fix text keying");
  gl_text_key k{0, 0};
  mark_t(k);
  if (!ts.contains(k)) ts[k] = gl_text(t, f, bg, fg);
  return ts[k];
}


gl_vbo &gl_render_state::vbo(Stc &n, ηic &xs)
{
  mark_v(n);
  if (!vs.contains(n)) TODO("vbo init");
  return vs[n];
}


gl_fbo_texture &gl_render_state::fbo(Stc &n, ηic &r)
{
  mark_f(n);
  if (!fs.contains(n)) TODO("fbo_texture init");
  return fs[n];
}


void gl_render_state::gc_begin()
{
  A(f, "gc_begin() called outside of begin()..end() block");
  mts.clear();
  mvs.clear();
  mfs.clear();
}


void gl_render_state::gc_end()
{
  A(f, "gc_end() called outside of begin()..end() block");

  V<gl_text_key> rts;
  V<St>          rvs;
  V<St>          rfs;

  for (auto &[k, v] : ts) if (!mts.contains(k)) rts.push_back(k);
  for (auto &[k, v] : vs) if (!mvs.contains(k)) rvs.push_back(k);
  for (auto &[k, v] : fs) if (!mfs.contains(k)) rfs.push_back(k);

  for (let &k : rts) ts.erase(k);
  for (let &k : rvs) vs.erase(k);
  for (let &k : rfs) fs.erase(k);

  mts.clear();
  mvs.clear();
  mfs.clear();
}


}

#include "end.hh"
