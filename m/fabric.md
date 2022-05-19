# Fabric
The Linux kernel is the IO fabric for a single system; likewise, the τ runtime provides IO fabric for a single τ node. There are two high-level approaches we might take:

1. Cooperative multithreading, e.g. using C++ coroutines
2. Preemptive multithreading, using a custom bytecode interpreter

(2) sounds bad, but we're already semi-committed to an intermediate layer given that most of the source isn't written in C++ (well, except for FFI). Also, we could generate a bytecode automatically by assembling fragments, just as we'd do if we transpiled for (1). So the real question isn't "how do we want to write code"; rather, it's "how should code interface with fabric".


## Portability
If we're transpiling anyway, we have a couple of nice things working for us:

1. We can get a lot of C++'s expressiveness by writing a smart transpiler
2. If we transpile to C, we can get a lot of portability
3. We can have multi-language compilation for full architecture independence

(3) is not as easy as it sounds; we need to manage the language runtime as well as write code for it, so it's a bit of overhead.

Still, though, all of this means we can just shell out to a C++ process (maybe with shared memory for communication!) if we need stuff like `std::unordered_map` or whatever other thing I don't want to write by hand.

In other words, let's make the fabric simple and build robust FFIs for languages. Nothing wrong with FIFOing data elements between the "hashmap component" and the rest of the app, or between the X11/SDL2 component and the rest of the app.


## C++ coroutines
Let's explore this option. I'm starting with [this tutorial](https://www.scs.stanford.edu/~dm/blog/c++-coroutines.html) to figure out the basic landscape.

Superficially, C++ coroutines look pretty clunky because they require lots of wrapping types that we're unlikely to want to work with.

For now I think we'll do better by writing atomic code patches that are adapted into an ad-hoc bytecode and run by an interpreter that manages coroutine-style state.
