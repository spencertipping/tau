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
  // TODO
}


void gl_render_state::run()
{
  for (let &x : rs.y()) apply(x);
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
