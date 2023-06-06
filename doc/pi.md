# π scripting
π is a series of [φ](phi.md) parsers that allow [η](eta.md)-transforming functions to be defined from string inputs, ultimately making it possible to write many τ programs without compiling code.

π uses a simple mark/sweep GC that inlines values when they are written into others. This means we have zero-overhead η reads and linear-overhead η consing with optimal data locality.

+ [π interpreter](pi-int.md)
+ [π grammar](pi-phi.md)
+ [π function](pi-fn.md)
+ [σ/π stdlib](sigma-pi-stdlib.md)


## Horizontal, vertical, and plurality
**TODO:** `πi` contains multiple values (vertical), each of which may have multiple `η` stream entries (horizontal). The question here is how we intend to address these values and possibly convert between them.
