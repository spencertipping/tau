#ifndef σgl_prims_h
#define σgl_prims_h

#include <GLES2/gl2.h>

#include "gl-geom.hh"
#include "begin.hh"

namespace σ
{

using namespace τ;


struct gl_usable
{
  virtual void use() = 0;
};

struct gl_null_usable : public virtual gl_usable
{
  void use() override {}
};


struct gl_fbo_texture final : public virtual gl_usable
{
  GLuint tid = 0;
  vec2   dims;

  gl_fbo_texture()                      = default;
  gl_fbo_texture(gl_fbo_texture const&) = default;
  gl_fbo_texture(gl_fbo_texture &&t_) : tid(t_.tid), dims(t_.dims)
    { t_.tid = 0; }

  gl_fbo_texture(vec2c &dims, F<void()> const &render);
  gl_fbo_texture(ηic &x);

  ~gl_fbo_texture() { if (tid) glDeleteTextures(1, &tid); }

  gl_fbo_texture &operator=(gl_fbo_texture &&t)
    { tid  = t.tid;
      dims = t.dims;
      t.tid = 0;
      return *this; }

  void use() override { glBindTexture(GL_TEXTURE_2D, tid); }
};


struct gl_uniform final
{
  GLuint loc = -1;
  GLint  size = 0;
  GLenum type = 0;

  gl_uniform()                  = default;
  gl_uniform(gl_uniform const&) = default;
  gl_uniform(gl_uniform &&u_) : loc(u_.loc), size(u_.size), type(u_.type)
    { u_.loc = -1; }

  gl_uniform(GLuint loc, GLint size, GLenum type)
    : loc(loc), size(size), type(type) {}

  gl_uniform &operator=(gl_uniform const&) = default;

  gl_uniform const &operator=(f64 x)     const { A(type == GL_FLOAT,      "uniform=f64");  glUniform1f(loc, x); return *this; }
  gl_uniform const &operator=(i64 x)     const { A(type == GL_INT,        "uniform=i64");  glUniform1i(loc, x); return *this; }
  gl_uniform const &operator=(vec2c &v)  const { A(type == GL_FLOAT_VEC2, "uniform=vec2"); glUniform2f(loc, v.x, v.y); return *this; };
  gl_uniform const &operator=(vec3c &v)  const { A(type == GL_FLOAT_VEC3, "uniform=vec3"); glUniform3f(loc, v.x, v.y, v.z); return *this; };
  gl_uniform const &operator=(vec4c &v)  const { A(type == GL_FLOAT_VEC4, "uniform=vec4"); glUniform4f(loc, v.x, v.y, v.z, v.w); return *this; }
  gl_uniform const &operator=(colorc &v) const { A(type == GL_FLOAT_VEC4, "uniform=vec4"); glUniform4f(loc, v.r, v.g, v.b, v.a); return *this; }
  gl_uniform const &operator=(mat4c &m)  const
    {
      A(type == GL_FLOAT_MAT4, "uniform=mat4");
      GLfloat f[16];
      for (int i = 0; i < 16; ++i) f[i] = m.m[i/4][i%4];
      glUniformMatrix4fv(loc, 16, false, f);
      return *this;
    }

  gl_uniform const &operator=(ηic &x) const
    {
      switch (type)
      {
        case GL_FLOAT:      return *this = x[0].cf();
        case GL_INT:        return *this = x[0].ci();
        case GL_FLOAT_VEC2: return *this = x[0].η();
        case GL_FLOAT_VEC3: return *this = x[0].η();
        case GL_FLOAT_VEC4: return *this = x[0].η();
        default: A(0, "uniform=unknown"); return *this;
      }
    }
};


struct gl_program final : public virtual gl_usable
{
  St     vertex;
  St     frag;
  GLuint vid = 0;
  GLuint fid = 0;
  GLuint pid = 0;
  M<St, gl_uniform> us;

  gl_program()                  = default;
  gl_program(gl_program const&) = default;
  gl_program(gl_program &&p_) { *this = mo(p_); }

  gl_program(Stc &vertex, Stc &frag);
  gl_program(ηic &x) : gl_program(x[0].cs(), x[1].cs()) {}

  ~gl_program();

  gl_program &operator=(gl_program &&p)
    { vertex = mo(p.vertex);
      frag   = mo(p.frag);
      vid    = p.vid;
      fid    = p.fid;
      pid    = p.pid;
      us     = mo(p.us);
      p.vid  = 0;
      p.fid  = 0;
      p.pid  = 0;
      return *this; }

  void use() override { glUseProgram(pid); }

  gl_uniform &operator[](Stc &name) { return us.at(name); }
};


struct gl_vbo final
{
  GLuint  vid    = 0;
  GLsizei vsize  = 0;
  GLsizei stride = 0;
  V<P<uN, uN>> as;

  gl_vbo() = default;
  gl_vbo(gl_vbo &&v_) { *this = mo(v_); }
  gl_vbo(ηic &x);

  ~gl_vbo() { if (vid) glDeleteBuffers(1, &vid); }

  gl_vbo &operator=(gl_vbo &&v)
    { vid    = v.vid;
      vsize  = v.vsize;
      stride = v.stride;
      as     = mo(v.as);
      v.vid  = 0;
      return *this; }

  void draw(GLenum mode = GL_TRIANGLES) const;
};


}

#include "end.hh"

#endif
