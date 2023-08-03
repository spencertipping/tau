#include <GLES2/gl2.h>
#include <stdexcept>

#include "gl-prims.hh"
#include "gl.hh"
#include "begin.hh"

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
  GLint r;
  glGetProgramiv(id, GL_LINK_STATUS, &r);

  if (r != GL_TRUE)
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


gl_fbo_texture::gl_fbo_texture(vec2c &dims, F<void()> const &render)
  : dims(dims)
{
  GLuint fbo;
  glGenFramebuffers(1, &fbo);
  glBindFramebuffer(GL_FRAMEBUFFER, fbo);

  glGenTextures(1, &tid);
  glBindTexture(GL_TEXTURE_2D, tid);
  glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA,
               dims.x, dims.y, 0, GL_RGBA, GL_UNSIGNED_BYTE, nullptr);
  glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0,
                         GL_TEXTURE_2D, tid, 0);

  glViewport(0, 0, dims.x, dims.y);
  glClearColor(0, 0, 0, 0);
  glClear(GL_COLOR_BUFFER_BIT);

  render();

  glBindFramebuffer(GL_FRAMEBUFFER, 0);
  glDeleteFramebuffers(1, &fbo);
}


gl_fbo_texture::gl_fbo_texture(ηic &x)
  : gl_fbo_texture({x.cf(), x[1].cf()},
                   [x = x[2]]()
                     {
                       gl_null_usable  u;
                       gl_render_state rs(&u);
                       rs.begin();
                       for (let &y : x) rs << y;
                       rs.end();
                     })
{}


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


gl_vbo::gl_vbo(ηic &x)
{
  // x = (((a1o a1l) (a2o a2l) ...)
  //      (x1 y1 z1 a1 a2 a3 ...)
  //      ...)
  // x[0] describes vertex attribute offsets

  glGenBuffers(1, &vid);
  glBindBuffer(GL_ARRAY_BUFFER, vid);

  let os = x[0].η();
  for (let &o : os)
  {
    let [a, l] = ηT<i64, i64>(o.η());
    as.push_back({a, l});
  }

  let vs = x.next();
  let n = vs.η().len();

  vsize  = vs.len();
  stride = sizeof(GLfloat) * n;
  f32 data[vsize * n];
  uN i = 0;
  for (let &v : vs)
  {
    let iv = v.η();
    uN j = 0;
    for (let &x : iv) data[i*n + j++] = x.cf();
  }

  glBufferData(GL_ARRAY_BUFFER, sizeof(f32) * vs.size() * n, data, GL_STATIC_DRAW);
}


void gl_vbo::draw(GLenum mode) const
{
  glEnableVertexAttribArray(0);
  glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, stride, 0);
  for (let &[a, l] : as)
  {
    glEnableVertexAttribArray(a);
    glVertexAttribPointer(a, l, GL_FLOAT, GL_FALSE, stride, (void *)(a * sizeof(GLfloat)));
  }

  glBindBuffer(GL_ARRAY_BUFFER, vid);
  glDrawArrays(mode, 0, vsize);
}


}

#include "end.hh"
