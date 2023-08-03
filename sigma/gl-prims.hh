#ifndef σgl_prims_h
#define σgl_prims_h

#include <GLES2/gl2.h>
#include <xxhash.h>

#include "gl-geom.hh"
#include "begin.hh"

namespace σ
{

using namespace τ;


struct gl_fbo_texture final
{
  GLuint tid = 0;
  vec2   dims;

  gl_fbo_texture()                      = default;
  gl_fbo_texture(gl_fbo_texture const&) = default;
  gl_fbo_texture(gl_fbo_texture &&t_) : tid(t_.tid), dims(t_.dims)
    { t_.tid = 0; }

  gl_fbo_texture(vec2c &dims, F<void()> const &render);

  ~gl_fbo_texture() { if (tid) glDeleteTextures(1, &tid); }

  void use() const { glBindTexture(GL_TEXTURE_2D, tid); }
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
};


struct gl_program final
{
  St     vertex;
  St     frag;
  GLuint vid = 0;
  GLuint fid = 0;
  GLuint pid = 0;
  M<St, gl_uniform> us;

  gl_program()                  = default;
  gl_program(gl_program const&) = default;
  gl_program(gl_program &&s_)
    : vertex(s_.vertex), frag(s_.frag), vid(s_.vid), fid(s_.fid), pid(s_.pid)
    { s_.vid = 0; s_.fid = 0; s_.pid = 0; }

  gl_program(Stc &vertex, Stc &frag);

  ~gl_program();

  void use() const { glUseProgram(pid); }

  gl_uniform &operator[](Stc &name) { return us.at(name); }
};


struct gl_vbo final
{
  V<vec3> vs;

  gl_vbo()              = default;
  gl_vbo(gl_vbo const&) = default;
  gl_vbo(gl_vbo &&v_) : vs(std::move(v_.vs)) {}

  Txs gl_vbo(Xs... xs) : vs({xs...}) {}

  void draw(GLenum mode = GL_TRIANGLES) const;
};


}

#include "end.hh"

#endif
