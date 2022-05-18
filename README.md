# Tau
+ [Stream semantics](m/stream.md)
+ [Frames and records](m/records.md)


## Next steps
Independent things that can be executed:

+ [x] Finalize prototype frame format
+ [ ] C++/SDL2 editor prototype
+ [ ] Fabric prototype
+ [ ] Operator class prototype


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
  + https://dev.to/noah11012/using-sdl2-2d-accelerated-renderering-1kcb

We should have **co-locality edges** and **flexible edges** -- that is, edge types should provide hints about the amount of serialization required to transfer data. Maybe even something like "L1 cache co-located" vs "memory co-located" vs "co-mapped (but perhaps separate thread)" vs "no affinity".
