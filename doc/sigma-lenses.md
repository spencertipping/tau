# σ lenses
`ni` just deals with TSV, which provides `A`..`Z` as field readers. Some operations like `,sgA` end up modifying a field value; the implied `B` argument (for the summation) is both read/write: a lens.

η provides a more complicated landscape. [Multiplexers](sigma-multiplex.md) might want to address several different lensed values:

+ `tuple-wrap-left`: `v x ↔ (x, v)`
+ `tuple-wrap-right`: `v x ↔ (v, x)`
+ `tuple-cons-left`: `(v...) x ↔ (x, v...)`
+ `tuple-cons-right`: `(v...) x ↔ (v..., x)`
+ `map-key`: `k {vs...} x ↔ {k x vs...}`
