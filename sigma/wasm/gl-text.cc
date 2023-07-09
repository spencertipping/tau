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

    let glc = Module['canvas'];
    let gl  = glc.getContext('webgl');
    let tid = gl.createTexture();
    gl.bindTexture(gl.TEXTURE_2D, tid);
    gl.texImage2D(gl.TEXTURE_2D, 0, gl.RGBA, gl.RGBA, gl.UNSIGNED_BYTE, c);
    gl.texParameteri(gl.TEXTURE_2D, gl.TEXTURE_MIN_FILTER, gl.LINEAR);
    gl.texParameteri(gl.TEXTURE_2D, gl.TEXTURE_MAG_FILTER, gl.LINEAR);
    HEAP32[$7 >> 2] = tid;
  },
  scratch_canvas, w, h,  // $0 .. $2
  f.c_str(), t.c_str(),  // $3, $4
  fg.hex().c_str(),      // $5
  bg.hex().c_str(),      // $6
  &tid);                 // $7

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
