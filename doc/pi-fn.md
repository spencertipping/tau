# π functions
π is an interpreted language that uses C++ objects to represent executable code. I'm not spending a lot of effort on compiler-style optimizations; most π functions are very simple, so it's mort important that (1) the objects are easy to manipulate and (2) they have _predictable_ if not _optimal_ performance characteristics.

`πf<N>` is an object that represents an action that modifies an interpreter by adjusting its stack size by `N` items, so `πf<1>` adds an item for example.


## State
`πf` is concatenative: `f | g` returns a function that first runs `f`, then runs `g`. We internally encode `πf` using a flat vector of `std::function`s, each of which modifies the interpreter object in some way.
