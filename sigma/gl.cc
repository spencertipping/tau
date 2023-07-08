#include "gl.hh"
#include "begin.hh"

#include <GLES2/gl2.h>
#include <stdexcept>

namespace σ
{


static void compile(GLuint id, Stc &source)
{
  chc *src[2] = {source.c_str(), nullptr};
  glShaderSource(id, 1, src, nullptr);
  glCompileShader(id);

  GLint r;
  glGetShaderiv(id, GL_COMPILE_STATUS, &r);
  if (r == GL_FALSE)
  {
    GLint l;
    Ss    err;
    glGetShaderiv(id, GL_INFO_LOG_LENGTH, &l);
    let log = new GLchar[l];
    glGetShaderInfoLog(id, l, nullptr, log);
    err << "Failed to compile shader: " << source << "\n" << log;
    delete[] log;
    throw std::runtime_error(err.str());
  }
}


static void link(GLuint id)
{
  GLint linkStatus;
  glGetProgramiv(id, GL_LINK_STATUS, &linkStatus);

  if (linkStatus != GL_TRUE)
  {
    GLint l;
    Ss    err;
    glGetProgramiv(id, GL_INFO_LOG_LENGTH, &l);
    let log = new GLchar[l];
    glGetProgramInfoLog(id, l, nullptr, log);
    err << "Failed to link shader program: " << log;
    delete[] log;
    throw std::runtime_error(err.str());
  }
}


static M<St, gl_uniform> gl_uniforms(GLuint program)
{
  M<St, gl_uniform> r;
  GLint n;
  glGetProgramiv(program, GL_ACTIVE_UNIFORMS, &n);
  for (GLuint i = 0; i < static_cast<GLuint>(n); ++i)
  {
    constexpr GLsizei max_name_length = 256;
    ch      name[max_name_length];
    GLsizei nl;
    GLint   s;
    GLenum  t;
    glGetActiveUniform(program, i, max_name_length, &nl, &s, &t, name);
    GLuint l = glGetUniformLocation(program, name);
    r[name] = gl_uniform{l, s, t};
  }
  return r;
}


gl_program::gl_program(Stc &vertex, Stc &frag)
  : vertex(vertex), frag(frag)
{
  vid = glCreateShader(GL_VERTEX_SHADER);
  fid = glCreateShader(GL_FRAGMENT_SHADER);
  pid = glCreateProgram();

  compile(vid, vertex);
  compile(fid, frag);

  glAttachShader(pid, vid);
  glAttachShader(pid, fid);
  link(pid);
  us = gl_uniforms(pid);
}


gl_program::~gl_program()
{
  if (vid) glDeleteShader(vid);
  if (fid) glDeleteShader(fid);
  if (pid) glDeleteProgram(pid);
}


void gl_vbo::draw(GLenum mode) const
{
  GLuint id;
  glGenBuffers(1, &id);
  glBindBuffer(GL_ARRAY_BUFFER, id);

  f32 data[vs.size() * 3];
  for (uN i = 0; i < vs.size(); ++i)
  {
    data[i*3+0] = vs[i].x;
    data[i*3+1] = vs[i].y;
    data[i*3+2] = vs[i].z;
  }

  glBufferData(GL_ARRAY_BUFFER, sizeof(f32) * vs.size() * 3, data, GL_STATIC_DRAW);
  glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(GLfloat), nullptr);
  glEnableVertexAttribArray(0);
  glDrawArrays(mode, 0, vs.size());

  glDeleteBuffers(1, &id);
}


}

#include "end.hh"
