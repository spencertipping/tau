# σ: the τ standard library
σ defines common utilities that make τ ergonomic and useful. This includes [Γ](doc/Gamma.md) components and [φ](doc/phi.md) parsers. The `σ` namespace doesn't include `τ` by default. Most programs don't need to import `τ`, as `σ` provides entry points for common operations.


## Γ processor symbol table
| Symbol  | Syntax | Description               |
|---------|--------|---------------------------|
| `a`     |        |                           |
| `b`     |        |                           |
| `c`     |        |                           |
| `d`     |        |                           |
| `e`     |        | shell command filter      |
| `f`     |        | tuple field transform     |
| `g`     |        | τ-grouped sort            |
| `h`     |        |                           |
| `i`     |        | η identity out            |
| `j`     |        | τ-grouped sorted join     |
| `k`     |        | η repeated out            |
| `l`     |        |                           |
| `m`     |        | π 1:1 map                 |
| `n`/`N` |        | numeric ι/ϊ               |
| `o`     |        |                           |
| `p`     |        | π program                 |
| `q`     |        | prefix for queues         |
| `r`/`R` |        | π row selector            |
| `s`     |        |                           |
| `t`     |        |                           |
| `u`     |        |                           |
| `v`     |        |                           |
| `w`/`W` |        | tuple/map horizontal join |
| `x`     |        |                           |
| `y`     |        |                           |
| `z`     |        |                           |

| Symbol | Syntax | Description                  |
|--------|--------|------------------------------|
| `+`    |        | τ-group append               |
| `-`    |        |                              |
| `*`    |        |                              |
| `/`    |        |                              |
| `.`    |        |                              |
| `,`    |        |                              |
| `!`    |        |                              |
| `@`    |        | Ξ subscript                  |
| `#`    |        |                              |
| `$`    |        |                              |
| `%`    |        | Γ union                      |
| `^`    |        | τ-group prepend              |
| `&`    |        |                              |
| `(`    |        | dynamic multiplexer          |
| `)`    |        | dynamic multiplexer          |
| `[`    |        | Γ grouping                   |
| `]`    |        | Γ grouping                   |
| `{`    |        | static multiplexer           |
| `}`    |        | static multiplexer           |
| `<`    |        | stream in from persistent    |
| `>`    |        | stream out to persistent     |
| `\|`   | Γp     | modify argument to right-cap |
| `\\`   | Γp     | modify argument to backward  |
| `_`    |        |                              |
| `=`    |        | prefix for full-duplex ops   |
| `~`    |        |                              |
| `'`    |        | register                     |
| `"`    |        | bounded buffer               |
| `:`    |        | broadcast                    |
| `;`    |        |                              |
| `?`    |        |                              |


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
