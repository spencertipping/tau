# Muon
Muon components are C++ `struct` wrappers around [γ](gamma.md) driver objects. Muon provides a small API to connect things easily.

**TODO:** to make this faster, let's use continuous buffers for UTF9 transit -- and in particular, let's `malloc()` and offload for large values, whereas small values are encoded inline. Then ζ is continuously GC'ing. To do this, we need a "is smaller than _x_" detector for UTF9 vals.
