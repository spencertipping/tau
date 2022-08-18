# Muon
Muon components are C++ `struct` wrappers around [γ](gamma.md) driver objects. Muon provides a small API to connect things easily.

**TODO:** to make this faster, let's use continuous buffers for UTF9 transit -- and in particular, let's `malloc()` and offload for large values, whereas small values are encoded inline. Then ζ is continuously GC'ing. To do this, we need a fast "serialize is cheap" detector for UTF9 vals.

**NOTE:** we can do two things to help out:

1. Parameterize ζ to specific non-UTF9 types
2. Add a non-Φ-portable `shared_ptr<>` variant to UTF9, effectively serializing `shared_ptr`s directly into the ζ output buffer (and destructing them when read)

If we add these natives to UTF9, we'll need to make sure they can't be nested into containers; otherwise any reference counting will probably be wrong. _Or_ we could more maintainably just maintain a local "heap" of stuff and explicitly destroy references once they're done.
