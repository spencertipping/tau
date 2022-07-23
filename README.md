# Tau
+ Fundamentals
  + [utf9](m/utf9.md)
  + [coro](m/coro.md)
  + [species](m/species.md)
+ Connection fabric
  + [trunk](m/trunk.md)
  + [leaf](m/leaf.md)
  + [cell](m/cell.md)
  + [port](m/port.md)
  + [chan](m/chan.md)
  + [pipe](m/pipe.md)


## Setup
For all of `dev/hackery` on Ubuntu 22.04:

```sh
$ sudo apt install -y \
       libboost-fiber-dev libboost-context-dev libmsgpack-dev \
       libxcb1-dev libx11-dev libx11-xcb-dev libgl-dev libxcb-xinput-dev
```


### Building test files
```sh
$ ./build t
```

**NOTE:** emscripten via `node` is ~50x slower than native at decoding UTF9. Shouldn't be a huge deal since most of the heavy lifting is done server-side, but emscripten is far from efficient.

**NOTE:** `wasm64` is enabled with `-sMEMORY64=1`, but it requires no exceptions and fails with asyncify. It also seems to load an experimental set of emscripten libraries -- I doubt we can make it work.


## Next steps
I think there will be two layers: first, we have the actual runtime mechanics; and second, we have the functions that compile into it. The abstractions can probably be pretty close to reality, but we'll probably want to focus on loop-connectivity and constraints, vs pins to specific runtimes.


### X11 UI
Stuff I should do in order to move forwards:

+ [ ] Vector sketching UI/language, incrementally-developed
  + [ ] Figure out how to render text into this
    + **Q:** do we need proper text, or can we DIY some glyphs? Probably the latter

**NOTE:** https://wdobbie.com/post/gpu-text-rendering-with-vector-textures/ lays it all out; https://github.com/azsn/gllabel/blob/master/lib/gllabel.cpp is an example driver for it

**NOTE:** XCB/xinput: https://gist.github.com/LemonBoy/dfe1d7ea428794c65b3d and https://stackoverflow.com/questions/39641675/how-to-register-events-using-libxcb-xinput


### X11/GL architecture
Each window is a single component within a central τ fabric. Each window owns a thread that produces XCB events on a queue; these forward to a τ channel unidirectionally. So an X11/GL component works like this from τ's perspective:

```
rendering/XCB commands ----> |
XCB/GL events <------------- | X11/GL window
                             |
```

**NOTE:** window size change is an XCB event, and it can be a command too. Frames might emit a synthetic XCB event when we request a size change, confirming that the new size is now active.

In practice, we're likely to have a viewstate register that incorporates model + camera. Then we'll clock-sync that to a stream of render commands, which establishes a framerate. If the register hasn't changed and no intervening event requires us to re-render, then we leave the window alone.

**Q:** what kind of data are we sending around? Discussion in [m/records.md](m/records.md).


### Rendering primitives
1. Shaders to compile
2. Buffers/uniforms/matrices to set
3. Triangles to render

Shaders can produce GL "shader error" outputs for debugging purposes. See [shader compilation](https://www.khronos.org/opengl/wiki/Shader_Compilation) for the C++ code canonically used to do this.


### Self-hosting path
+ [ ] Factor the UI shell into a τ component
  + [ ] Needs to work with the multithreaded frontend
  + [ ] Define UI state and GL projection component network
  + [ ] Define visual topology rendering
+ [ ] Define instantiation grammar
+ [ ] Create initial fabric (with realtime scheduling)
  + [ ] Drive rendering with fabric events
  + [ ] Async inbound XCB events


## Updates
1. CAD = code; in CAD we use dimensional constraints and specific distances, but _we maintain topology_ -- code is strictly topological
2. CAD = code = serial input -- for code, the traversal order is pre-order on parse trees
3. The editor/viewer is a 3D C++ program whose input is the key transcript (and the editor is a natural fit for being written in tau)
4. Within the transcript format, `[enter]` resets context such that the next op will be locality-prefixed (which makes `git diff` work as expected)
5. For UX, think [cheloniidae](https://spencertipping.com/cheloniidae), but with rendering overlays for discrete elements
6. Nodes are C++ classes with green-threaded fabric mediation

More notes:

+ XCB programming
  + https://www.x.org/releases/X11R7.7/doc/libxcb/tutorial/index.html

We should have **co-locality edges** and **flexible edges** -- that is, edge types should provide hints about the amount of serialization required to transfer data. Maybe even something like "L1 cache co-located" vs "memory co-located" vs "co-mapped (but perhaps separate thread)" vs "no affinity".
