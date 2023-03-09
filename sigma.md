# σ: the τ standard library
σ defines common utilities that make τ ergonomic and useful. This includes [Γ](doc/Gamma.md) components and [φ](doc/phi.md) parsers. The `σ` namespace doesn't include `τ` by default. Most programs don't need to import `τ`, as `σ` provides entry points for common operations.


## Γ symbol table
| Symbol | Syntax | Description |
|--------|--------|-------------|
| `a`    |        |             |
| `b`    |        |             |
| `c`    |        |             |
| `d`    |        |             |
| `e`    |        |             |
| `f`    |        |             |
| `g`    |        |             |
| `h`    |        |             |
| `i`    |        |             |
| `j`    |        |             |
| `k`    |        |             |
| `l`    |        |             |
| `m`    |        |             |
| `n`    |        |             |
| `o`    |        |             |
| `p`    |        |             |
| `q`    |        |             |
| `r`    |        |             |
| `s`    |        |             |
| `t`    |        |             |
| `u`    |        |             |
| `v`    |        |             |
| `w`    |        |             |
| `x`    |        |             |
| `y`    |        |             |
| `z`    |        |             |

| Symbol | Syntax | Description |
|--------|--------|-------------|
| `+`    |        |             |
| `-`    |        |             |
| `*`    |        |             |
| `/`    |        |             |
| `!`    |        |             |
| `@`    |        |             |
| `#`    |        |             |
| `$`    |        |             |
| `%`    |        |             |
| `^`    |        |             |
| `&`    |        |             |
| `*`    |        |             |
| `(`    |        |             |
| `)`    |        |             |
| `[`    |        |             |
| `]`    |        |             |
| `{`    |        |             |
| `}`    |        |             |
| `      | `      |             |
| `_`    |        |             |
| `=`    |        |             |
| `~`    |        |             |
| `'`    |        |             |
| `"`    |        |             |
| `:`    |        |             |
| `;`    |        |             |
| `<`    |        |             |
| `>`    |        |             |
| `,`    |        |             |
| `.`    |        |             |
| `?`    |        |             |


## TODO
+ Standard data-transform library
  + `ni f` equivalent
  + `ni r` equivalent?
+ Remove unicode from common operations
  + Unicode should be reserved for things you'd use in scripts, e.g. variables
  + Unicode with ASCII digraphs may be fine
+ HTTP library
  + HTTP/REST _requests_ are a stream, which can be sub-streamed for WS
  + Authentication is a fork
+ π scripting
  + ← for Γ assignment, refer to them later
+ Generalize Ξ topology in Γ layer
  + Any `a → b` should be forward, backward, left-loop, or right-loopable
  + Do this by having Γ2 with different `Ξ → P<ξi, ξo>` functions
  + We can generalize further by having other Γ numbers
