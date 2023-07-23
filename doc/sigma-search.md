# Incremental search/traversal
**TODO:** define the result of a search/traversal


The goal is to generalize breadth-first, depth-first, and prioritized searches in such a way that we can operate incrementally. The structure of the search is determined by the dependencies between nodes. For example, `du` is a depth-first search because parent directory results depend on their children. `find` could go either way, but is often depth-first for efficiency.

τ complicates the picture for search because it breaks ordering guarantees that are present in time-domain languages. We must address the data logically and accept out-of-order results. In the `du` case, for example, our directory-traversal function might be parallelized and explore multiple toplevel directories at once (suppose this makes sense from a performance perspective). Then requests and results for descendants would arrive jumbled, leaving the search operator to associate them back to their parents.

We can generalize all of this by breaking searches into two pieces:

1. The expander, which visits a node and describes its prioritized neighbors and dependencies
2. The collapser, which collects dependency results and produces the result for the node

For example, `du` would expand by listing directory entries and would collapse by returning the size for files and the summed child size for directories. `du` is entirely depth-first, so there is no relative priority between child entries.


## Dependencies and priority
If _A_ depends on _B_, then _B_'s real priority will be at least as large as _A_'s. We do this by adding _A_'s priority to _B_. This isn't a perfect system, but it covers most of the cascading we need. In particular, it's likely to balance parallelism with dependency resolution.


## Caching and restarting
Search state -- and by this I specifically mean results -- can be cached to disk using SQLite or another persistent [container](sigma-containers.md). If a search begins when the cache is prepopulated, then it will proceed normally and discover results that already exist, avoiding work.

Internally, searching involves keeping track of nodes that have been sent to the expander so we don't expand the same node more than once, e.g. if a node is a dependency of multiple others. This is reset each time the search begins, since it reflects the state of active work, which is more volatile than the result container.


**Q:** should we require that neighbors and dependencies be terminated by _τ_? If we don't, then we have a continuous search.
