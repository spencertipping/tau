# Tau
+ [Stream semantics](m/stream.md)
+ [Records](m/records.md)
+ [Connection fabric](m/fabric.md)
+ [General-purpose transforms](m/transform.md)


## Proprietary data; keep closed-source


## Setup
For all of `dev/hackery`:

```sh
$ sudo apt install -y libboost-fiber-dev libmsgpack-dev libxcb1-dev
```


## Next steps
Stuff I should do in order to move forwards:

+ [x] Figure out how to get transparent frames (`libxcb` probably)
  + [x] Figure out how to get X11/GL rendering
  + [x] Figure out how to get X11/GL window transparency
  + [x] Render some geometry
+ [ ] Vector sketching UI/language, incrementally-developed
  + [ ] Figure out how to render text into this

Independent things that _can_ be executed:

+ [x] Finalize prototype record format
+ [ ] Fabric prototype (note that we'll probably rewrite the original)
  + [ ] Hand-code an initial stream process
  + [ ] Experiment with coercions to/from records
+ [ ] Operator class prototype
+ [ ] C++/SDL2 editor prototype


## Compilation model
`tau` is more of a meta-language than a language, in that the first stage is to compile operator definitions (which are parser specs and C++ backing code) into an executable that can then parse its input and construct the dataflow graph. We might have a three-stage process:

1. Perl to compile the C++ image
2. That image consumes 2D ASCII source and produces another image
3. That image supports graphical programming and a more complex file structure


## Architecture?
The "tau node" is a process, which streams a boot series of [records](m/records.md) to configure its behavior. Then "tau the language" is just a transform from source to these boot records.

In other words, the node initially streams to the fabric; then the boot program redirects input to a specific stream, which begins proper computation.

Tau nodes can spin up other nodes and send file descriptors around, creating the full topology.


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
+ C++ coroutine tutorial https://www.scs.stanford.edu/~dm/blog/c++-coroutines.html
+ XCB programming
  + https://www.x.org/releases/X11R7.7/doc/libxcb/tutorial/index.html

We should have **co-locality edges** and **flexible edges** -- that is, edge types should provide hints about the amount of serialization required to transfer data. Maybe even something like "L1 cache co-located" vs "memory co-located" vs "co-mapped (but perhaps separate thread)" vs "no affinity".
