#if τgl

#include <GLES2/gl2.h>
#include <stdexcept>

#include "gl.hh"
#include "begin.hh"

namespace σ
{


gl_render_state &gl_render_state::begin()
{
  f = true;
  t->use();
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
  let s = x[0].n().x;
  let c = x.next();

  t->use();

  if      (s == "v") vbo(c).draw();
  else if (s == "f") fbo(c).use();
  else if (s == "p") prog(c).use();
  else if (s == "t") text(c).use();
  else if (s == "u") uniforms(c);
  else A(0, "unknown rop: " << x);
}


void gl_render_state::run()
{
  for (let &x : rs.y()) apply(x.η());
}


gl_text &gl_render_state::text(ηic &x)
{
  // text("hi there" "Ubuntu Mono 10" (0 0 0 1) (1 1 1 1))
  let h = x.sha256();
  mark_t(h);
  if (!ts.contains(h)) ts[h] = gl_text(x);
  return ts[h];
}


gl_vbo &gl_render_state::vbo(ηic &x)
{
  // vbo(name (a1o a1l a2o a2l ...) (x1 y1 z1 a1 a2 ...) ...)
  let h = x.one().sha256();
  mark_v(h);
  if (!vs.contains(h)) vs[h] = gl_vbo(x.next());
  return vs[h];
}


gl_fbo_texture &gl_render_state::fbo(ηic &x)
{
  // fbo(name w h rops...)
  let h = x.one().sha256();
  mark_f(h);
  if (!fs.contains(h)) fs[h] = gl_fbo_texture(x.next());
  return fs[h];
}


gl_program &gl_render_state::prog(ηic &x)
{
  // prog(name vs fs)
  let h = x.one().sha256();
  mark_p(h);
  if (!ps.contains(h))
  {
    A(x.next(), "prog() requires vertex and fragment shaders");
    ps[h] = gl_program(x.next());
  }
  return ps[h];
}


void gl_render_state::uniforms(ηic &x)
{
  // uniforms(name 'n1 v1 'n2 v2 ...) -- prog must already be bound
  let &p = prog(x.one());
  for (let &y : x.next())
    if (y.is_n())
      p.us.at(y.n().x) = y.next().one();
}


void gl_render_state::clear_marks()
{
  mts.clear();
  mvs.clear();
  mfs.clear();
  mps.clear();
}


void gl_render_state::gc_begin()
{
  A(f, "gc_begin() called outside of begin()..end() block");
  clear_marks();
}


void gl_render_state::gc_end()
{
  A(f, "gc_end() called outside of begin()..end() block");

  V<h256> rts;
  V<h256> rvs;
  V<h256> rfs;
  V<h256> rps;

  rts.reserve(ts.size());
  rvs.reserve(vs.size());
  rfs.reserve(fs.size());
  rps.reserve(ps.size());

  for (auto &[k, v] : ts) if (!mts.contains(k)) rts.push_back(k);
  for (auto &[k, v] : vs) if (!mvs.contains(k)) rvs.push_back(k);
  for (auto &[k, v] : fs) if (!mfs.contains(k)) rfs.push_back(k);
  for (auto &[k, v] : ps) if (!mps.contains(k)) rps.push_back(k);

  for (let &k : rts) ts.erase(k);
  for (let &k : rvs) vs.erase(k);
  for (let &k : rfs) fs.erase(k);
  for (let &k : rps) ps.erase(k);

  clear_marks();
}


}

#include "end.hh"

#endif
