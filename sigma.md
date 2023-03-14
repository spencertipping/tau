# σ: the τ standard library
σ defines common utilities that make τ ergonomic and useful. This includes [Γ](doc/Gamma.md) components and [φ](doc/phi.md) parsers. The `σ` namespace doesn't include `τ` by default. Most programs don't need to import `τ`, as `σ` provides entry points for common operations.

Structurally, an application is a half-duplex processing edge that consumes `stdin` and produces `stdout`. The processing edge is allowed to allocate subgraphs comprised of complex topologies, each edge of which is its own stream processor (which can break down into further sub-topologies, etc).

There are two ways we define topologies:

1. Explicitly and up-front, with references to stream processors
2. Implicitly as side-taps from a chain, e.g. debug/monitor outputs

Notationally, this means we define topology and processing at the same time in general -- but with some out-of-line elements.

**NOTE:** we need to be able to instantiate a ψ and create a Γ that refers specifically to that ψ. This is effectively a pinned global data processor that goes out of scope when all referring Γs are deallocated. These boxed ψs must be of a subclass that provides `operator()(Ξ&)` to accept new hookups.


## Γ processor symbol table
| Symbol  | Prefix? | Syntax | Description               |
|---------|---------|--------|---------------------------|
| `a`     |         |        |                           |
| `b`     |         |        |                           |
| `c`     |         |        | count adjacent            |
| `d`     |         |        |                           |
| `e`     | P       |        | prefix for shell commands |
| `f`     |         |        | tuple field transform     |
| `g`     |         |        | τ-grouped sort            |
| `h`     |         |        | HTTP/S+WS client          |
| `i`     |         |        | η identity out            |
| `j`     |         |        | τ-grouped sorted join     |
| `k`     |         |        | τ after each item         |
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
| `z`     | P       |        | prefix for compression    |

| Symbol  | Prefix? | Syntax | Description                   |
|---------|---------|--------|-------------------------------|
| `A`     | P       |        | prefix for authentication     |
| `B`     | P       |        | prefix for bounded buffers    |
| `C`     |         |        |                               |
| `D`     | P       |        | prefix for decryption         |
| `E`     | P       |        | prefix for encryption         |
| `F`     | P       |        | prefix for formatted IO       |
| `G`     |         |        | stateful gate                 |
| `H`     |         |        | HTTP+WS server                |
| `I`     |         |        | η repeated out                |
| `J`     |         |        | τ-grouped unordered join      |
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

**Q:** what does "eager annotation" mean?

| Symbol  | Prefix? | Syntax | Description                     |
|---------|---------|--------|---------------------------------|
| `&`     | P       |        | prefix for eager annotation     |
| `@`     | P       |        | prefix for sqlite               |
| `#`     | P       |        | prefix for other DBs            |
| `~`     | P       |        | prefix for delay and rate-limit |
| `<`     | P       |        | prefix for unboxing             |
| `>`     | P       |        | prefix for boxing               |
| `?`     | P       |        | prefix for lossy queues         |
| `/`     | P       |        | platform-specific namespace     |
| `!`     |         |        | evaluate a Γ to a static ψ      |
| `+`     |         |        | τ-group append                  |
| `=`     |         |        |                                 |
| `-`     |         |        |                                 |
| `*`     |         |        |                                 |
| `'`     |         |        |                                 |
| `"`     |         |        |                                 |
| `←`     |         | n v    | define variable                 |
| `$`     |         |        | refer to variable               |
| `.`     |         |        | Ξ subscript                     |
| `,`     |         |        | ξ bundle (collect into Ξ)       |
| `%`     |         |        | Γ union (eager interleave)      |
| `^`     |         |        | τ-group prepend                 |
| `(`     |         |        | dynamic demultiplexer           |
| `)`     |         |        | dynamic multiplexer             |
| `[`/`]` |         |        | Γ grouping                      |
| `{`     |         |        | static demultiplexer            |
| `}`     |         |        | static multiplexer              |
| `\|`    |         | Γp     | modify processor to right-cap   |
| `\\`    |         | Γp     | modify processor to backward    |
| `_`     |         |        | null port                       |
| `:`     |         |        | omni-blocking broadcast         |
| `;`     |         |        | non-blocking side tap           |


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
