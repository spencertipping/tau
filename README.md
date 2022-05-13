# Tau
+ [Stream operation bytecode](m/bytecode.md)
+ [Stream semantics](m/stream.md)
+ [Negotiation and IO](m/negotiation.md)
+ [Frames and records](m/records.md)
+ [C++ API](m/cpp.md)


## Main components
1. Multi-threaded native fabric with boundary `epoll` IO (single `epoll` proxy thread, probably)
2. Multi-worker wasm fabric
3. Fabric configuration directives

Multithreaded native processes will have one IO-boundary process and all others will be simple workers using utf9 messaging within shared memory to communicate.

**Q:** what type of GC is appropriate?

**Q:** what is our core execution bytecode?

**NOTE:** both execution and pipeline sections can be parsed with PEGs


## Updates
1. CAD = code; in CAD we use dimensional constraints and specific distances, but _we maintain topology_ -- code is strictly topological
2. CAD = code = serial input -- for code, the traversal order is pre-order on parse trees
3. The editor/viewer is a 3D C++ program whose input is the key transcript (and the editor is a natural fit for being written in tau)
4. Within the transcript format, `[enter]` resets context such that the next op will be locality-prefixed (which makes `git diff` work as expected)
5. For UX, think [cheloniidae](https://spencertipping.com/cheloniidae), but with rendering overlays for discrete elements
6. Nodes are C++ classes with green-threaded fabric mediation

More notes:

+ C++/native should use SDL2 (which provides shader support)
  + http://wiki.libsdl.org/Introduction
  + https://www.jamesfmackenzie.com/2019/12/01/webassembly-graphics-with-sdl/
  + https://glusoft.com/tutorials/sdl2/use-glsl-shader
