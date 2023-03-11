# σ: the τ standard library
σ defines common utilities that make τ ergonomic and useful. This includes [Γ](doc/Gamma.md) components and [φ](doc/phi.md) parsers. The `σ` namespace doesn't include `τ` by default. Most programs don't need to import `τ`, as `σ` provides entry points for common operations.


## Γ processor symbol table
| Symbol  | Prefix? | Syntax | Description               |
|---------|---------|--------|---------------------------|
| `a`     |         |        |                           |
| `b`     |         |        |                           |
| `c`     |         |        | count adjacent            |
| `d`     |         |        |                           |
| `e`     |         |        | shell command filter      |
| `f`     |         |        | tuple field transform     |
| `g`     |         |        | τ-grouped sort            |
| `h`     |         |        | HTTP/S+WS client          |
| `i`     |         |        | η identity out            |
| `j`     |         |        | τ-grouped sorted join     |
| `k`     |         |        |                           |
| `l`     |         |        |                           |
| `m`     |         |        | π 1:1 map                 |
| `n`/`N` |         |        | numeric ι/ϊ               |
| `o`     |         |        |                           |
| `p`     |         |        | π program                 |
| `q`     |         |        | prefix for queues         |
| `r`/`R` |         |        | π row selector            |
| `s`     |         |        |                           |
| `t`     |         |        | TCP client                |
| `u`     |         |        | UDP client                |
| `v`     |         |        | UNIX domain socket client |
| `w`/`W` |         |        | tuple/map horizontal join |
| `x`     |         |        |                           |
| `y`     |         |        |                           |
| `z`     |         |        |                           |

| Symbol  | Prefix? | Syntax | Description                   |
|---------|---------|--------|-------------------------------|
| `A`     | P       |        | prefix for authentication     |
| `B`     |         |        |                               |
| `C`     |         |        |                               |
| `D`     | P       |        | prefix for decryption         |
| `E`     | P       |        | prefix for encryption         |
| `F`     | P       |        | prefix for formatted IO       |
| `G`     |         |        | stateful gate                 |
| `H`     |         |        | HTTP+WS server                |
| `I`     |         |        | η repeated out                |
| `J`     |         |        |                               |
| `K`     |         |        | τ repeater                    |
| `L`     |         |        |                               |
| `M`     |         |        | monitor                       |
| `N`/`n` |         |        | numeric ϊ/ι                   |
| `O`     |         |        |                               |
| `P`     |         |        |                               |
| `Q`     |         |        |                               |
| `R`/`r` |         |        | π row selector                |
| `S`     | P       |        | prefix for horizontal scaling |
| `T`     |         |        | TCP server                    |
| `U`     |         |        | UDP server                    |
| `V`     |         |        | UNIX domain socket server     |
| `W`/`w` |         |        | tuple/map horizontal join     |
| `X`     | P       |        | prefix for OT integral        |
| `Y`     | P       |        | prefix for OT derivative      |
| `Z`     | P       |        | prefix for OT                 |

| Symbol | Prefix? | Syntax | Description                   |
|--------|---------|--------|-------------------------------|
| `!`    | P       |        | prefix for lossy queues       |
| `=`    | P       |        | prefix for full-duplex ops    |
| `&`    | P       |        | prefix for eager annotation   |
| `@`    | P       |        | prefix for sqlite             |
| `#`    | P       |        | prefix for other DBs          |
| `+`    |         |        | τ-group append                |
| `-`    |         |        |                               |
| `*`    |         |        |                               |
| `/`    |         |        |                               |
| `~`    |         |        |                               |
| `$`    |         |        |                               |
| `.`    |         |        | Ξ subscript                   |
| `,`    |         |        | ξ bundle (collect into Ξ)     |
| `%`    |         |        | Γ union (eager interleave)    |
| `^`    |         |        | τ-group prepend               |
| `(`    |         |        | dynamic demultiplexer         |
| `)`    |         |        | dynamic multiplexer           |
| `[`    |         |        | Γ grouping                    |
| `]`    |         |        | Γ grouping                    |
| `{`    |         |        | static demultiplexer          |
| `}`    |         |        | static multiplexer            |
| `<`    | P       |        | stream in from persistent     |
| `>`    | P       |        | stream out to persistent      |
| `\|`   |         | Γp     | modify processor to right-cap |
| `\\`   |         | Γp     | modify processor to backward  |
| `_`    |         |        | null port                     |
| `'`    |         |        | register                      |
| `"`    |         |        | bounded buffer                |
| `:`    |         |        | omni-blocking broadcast       |
| `;`    |         |        | non-blocking side tap         |
| `?`    |         |        | debug tap                     |


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
