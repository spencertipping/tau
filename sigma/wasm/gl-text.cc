#include <emscripten.h>
#include <emscripten/html5.h>

#include "gl.hh"
#include "../begin.hh"

namespace σ
{


sletc scratch_canvas = "#tau-gl-text-scratch";


GLuint gl_text::texture()
{
  if (tid) return tid;

  let d = measure();
  let w = int(d.x);
  let h = int(d.y);
  let b = new u8[w * h * 4];

  EM_ASM(
  {
    let c   = document.getElementById(UTF8ToString($0));
    let ctx = c.getContext('2d');
    c.width  = $1;
    c.height = $2;
    ctx.font = UTF8ToString($3);
    ctx.clearRect(0, 0, c.width, c.height);
    ctx.fillStyle = UTF8ToString($6);
    ctx.fillRect(0, 0, c.width, c.height);
    ctx.fillStyle = UTF8ToString($5);
    ctx.fillText(UTF8ToString($4), 0, c.height);

    let id = ctx.getImageData(0, 0, c.width, c.height);
    let d  = id.data;
    for (let i = 0; i < d.length; i++) HEAPU8[$7 + i] = d[i];
  },
  scratch_canvas, w, h,  // $0 .. $2
  f.c_str(), t.c_str(),  // $3, $4
  fg.hex().c_str(),      // $5
  bg.hex().c_str(),      // $6
  b);

  glGenTextures(1, &tid);
  glBindTexture(GL_TEXTURE_2D, tid);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
  glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, w, h, 0, GL_RGBA, GL_UNSIGNED_BYTE, b);

  delete[] b;
  return tid;
}


vec2 gl_text::measure()
{
  EM_ASM(
  {
    let c    = document.getElementById(UTF8ToString($0));
    let ctx  = c.getContext('2d');
    ctx.font = UTF8ToString($1);
    let m    = ctx.measureText(UTF8ToString($2));
    HEAPF64[$3 >> 3] = m.width;
    HEAPF64[$4 >> 3] = m.actualBoundingBoxAscent + m.actualBoundingBoxDescent;
  }, scratch_canvas, f.c_str(), t.c_str(), &dims.x, &dims.y);
  return dims;
}


}

#include "../end.hh"
