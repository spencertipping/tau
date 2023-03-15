# σ lenses
`ni` just dealt with TSV, which provided `A`..`Z` as field readers. Some operations like `,sgA` would end up writing a field value, so in that sense the implied `B` argument (for the summation) was both read/write: a lens.

η provides a more complicated landscape. [Multiplexers](sigma-multiplex.md) might want to address several different lensed values:

+ `tuple-wrap-left`: `v ⇒ x ↔ (x, v)`
+ `tuple-wrap-right`: `v ⇒ x ↔ (v, x)`
+ `tuple-prepend`: `(v...) ⇒ x ↔ (x, v...)`
+ `map-key`: `k {...} ⇒ x ↔ {k x ...}`
