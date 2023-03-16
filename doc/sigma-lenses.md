# σ lenses
`ni` just deals with TSV, which provides `A`..`Z` as field readers. Some operations like `,sgA` end up modifying a field value; the implied `B` argument (for the summation) is both read/write: a lens.

η provides a more complicated landscape. [Multiplexers](sigma-multiplex.md) might want to address several different lensed values:

+ `tuple-wrap-left`: `v x ↔ (x, v)`
+ `tuple-wrap-right`: `v x ↔ (v, x)`
+ `tuple-cons-left`: `(v...) x ↔ (x, v...)`
+ `tuple-cons-right`: `(v...) x ↔ (v..., x)`
+ `map-key`: `k {vs...} x ↔ {k x vs...}`


## Lens symbol table
| Symbol   | Description        |
|----------|--------------------|
| `[`/`]`  | Lens grouping      |
| `(`      | `tuple-wrap-left`  |
| `)`      | `tuple-wrap-right` |
| `^`      | `tuple-cons-left`  |
| `+`      | `tuple-cons-right` |
| `a`..`z` | `tuple-element`    |
| `.k`     | `map-key(k)`       |

**NOTE:** lenses are composable by concatenation: `a.foo` refers to the `foo` key of the first element of a tuple; `ab` is `η[0][1]`. Whitespace is required to refer to a tuple element within a map key: `.foo a`.
