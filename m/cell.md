# Cell
A [leaf](leaf.md) contains many cells, each of which is an atomic data processor that uses one or more [coros](coro.md) to handle data moving through its [ports](port.md).

Each cell has a [species](species.md) that determines what it does and which ports it provides.


## Inner API
You create a cell by writing its internals, which means you inherit from `cell` and provide some method implementations.
