# Spatial computation
**Q:** what is the total effect space? e.g for τ in general it's very bounded: ψ can only emit on ξ. We need to define something similar here.

**Q:** how do we accommodate "populate a database" as an intended effect?

**Idea:** _G → k τ[...]_: keyed _τ_ groups to explore, then _k ... → A → v_ to calculate final values

**TODO:** what does it mean to linearize a spatial domain, especially given that we apparently also want to deduplicate it on the exploration side?


## How about this
The goal of any `?` operator is to handle topology and eliminate loops. So we should be able to define functionality in a "stream of node visits → stream of next steps" kind of way. Some next steps for a node _n_:

+ Emit _n_ with score _s_
+ Terminate at _n_
+ Enqueue _n'_ on frontier with priority _p_
+ Define dependency _n → n'_
+ Define result/attribute for _n_ (we need this to define dependencies)
+ Cache return value for _n_

**Q:** how do we specify what to do with dependency data? For example, `du` involves summing the child sizes; so we might stream out _n → n'_ for each child, then _v ← +/x_ or something.
