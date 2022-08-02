# Emscripten notes
Generally, `dev/emsdk em++ ...` is similar to `g++ ...`. However, there are some exceptions, starting with the fact that **emscripten builds are 32-bit, not 64-bit.** Additionally:

+ emscripten via `node` is ~50x slower than native at decoding UTF9. Shouldn't be a huge deal since most of the heavy lifting is done server-side, but emscripten is far from efficient.
+ `wasm64` is enabled with `-sMEMORY64=1`, but it requires no exceptions and fails with asyncify. It also seems to load an experimental set of emscripten libraries -- I doubt we can make it work.
