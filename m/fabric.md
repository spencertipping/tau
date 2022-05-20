# Fabric
Fabric bidirectionally mediates IO, which involves running critical sections of code. This is often done with coroutines, but those handle only the control-flow aspect of the problem. τ is better served by having objects that are (1) guaranteed to be used in a single-threaded way, and (2) respond to specific method calls. This means we aren't tied to any language's coroutine model, and each method is a critical section.


## C++ coroutines
Let's explore this option. I'm starting with [this tutorial](https://www.scs.stanford.edu/~dm/blog/c++-coroutines.html) to figure out the basic landscape.
