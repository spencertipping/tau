# Incremental search/traversal
The goal is to generalize breadth-first, depth-first, and prioritized searches in such a way that we can operate incrementally. The structure of the search is determined by the dependencies between nodes. For example, `du` is a depth-first search because parent directory results depend on their children. `find` could go either way, but is often depth-first for efficiency.

We can generalize all search and traversal with two variables:

1. Traversal ordering/priority
2. Termination conditions

For example, depth-first search is one where we visit children before neighbors; this is a matter of priority. In general, dependencies will have higher priority than adjacent things on the grounds that they represent things we've committed to.
