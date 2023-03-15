# σ: the τ standard library
σ defines common utilities that make τ ergonomic and useful. This includes [Γ](doc/Gamma.md) components and [φ](doc/phi.md) parsers. The `σ` namespace doesn't include `τ` by default. Most programs don't need to import `τ`, as `σ` provides entry points for common operations.

Structurally, an application is a half-duplex processing edge that consumes `stdin` and produces `stdout`. The processing edge is allowed to allocate subgraphs comprised of complex topologies, each edge of which is its own stream processor (which can break down into further sub-topologies, etc).

There are two ways we define topologies:

1. Explicitly and up-front, with references to stream processors
2. Implicitly as side-taps from a chain, e.g. debug/monitor outputs

Notationally, this means we define topology and processing at the same time in general -- but with some out-of-line elements.

**NOTE:** we need to be able to instantiate a ψ and create a Γ that refers specifically to that ψ. This is effectively a pinned global data processor that goes out of scope when all referring Γs are deallocated. These boxed ψs must be of a subclass that provides `operator()(Ξ&)` to accept new hookups. We do this with `!`.


## Complex topologies
+ [Lenses](sigma-lenses.md)
+ [Multiplexers and duplex connections](sigma-multiplex.md)


## Γ topology symbol table
| Symbol  | Syntax | Description                       |
|---------|--------|-----------------------------------|
| `[`/`]` |        | Γ grouping                        |
| `(`/`)` | π[Γ]   | dynamic half-duplex demultiplexer |
| `{`/`}` | k Γ    | static full-duplex demultiplexer  |
| `\|`    | Γp     | modify processor to right-cap     |
| `\\`    | Γp     | modify processor to backward      |


## Γ processor table
| Symbol  | Notes | Description               |
|---------|-------|---------------------------|
| `a`     |       |                           |
| `b`     |       |                           |
| `c`     |       | count adjacent            |
| `d`     |       |                           |
| `e`     | P     | prefix for shell commands |
| `f`     |       | tuple field transform     |
| `g`     |       | τ-grouped sort            |
| `h`     |       | HTTP/S+WS client          |
| `i`     |       | η identity out            |
| `j`     |       | τ-grouped sorted join     |
| `k`     |       | τ after each item         |
| `l`     |       |                           |
| `m`     |       | π 1:1 map                 |
| `n`/`N` |       | numeric ι/ϊ               |
| `o`     |       |                           |
| `p`     | π     | π program                 |
| `q`     |       | prefix for queues         |
| `r`/`R` | π     | π row selector            |
| `s`     |       |                           |
| `t`     |       | TCP client                |
| `u`     |       | UDP client                |
| `v`     |       | UNIX domain socket client |
| `w`/`W` | Γ     | tuple/map horizontal join |
| `x`     |       | exchange tuple fields     |
| `y`     |       |                           |
| `z`     | P     | prefix for compression    |

| Symbol  | Prefix? | Full? | Description                   |
|---------|---------|-------|-------------------------------|
| `A`     | P       | F     | prefix for authentication     |
| `B`     | P       |       | prefix for bounded buffers    |
| `C`     |         |       |                               |
| `D`     | P       | F     | prefix for decryption         |
| `E`     | P       | F     | prefix for encryption         |
| `F`     | P       |       | prefix for formatted IO       |
| `G`     |         |       | stateful gate                 |
| `H`     | Γ       |       | HTTP+WS server                |
| `I`     |         |       | η repeated out                |
| `J`     |         |       | τ-grouped unordered join      |
| `K`     |         |       | τ repeater                    |
| `L`     |         |       |                               |
| `M`     |         |       | monitor                       |
| `N`/`n` |         |       | numeric ϊ/ι                   |
| `O`     |         |       |                               |
| `P`     |         |       |                               |
| `Q`     |         |       |                               |
| `R`/`r` | π       |       | π row selector                |
| `S`     | P       |       | prefix for horizontal scaling |
| `T`     | Γ       |       | TCP server                    |
| `U`     | Γ       |       | UDP server                    |
| `V`     | Γ       |       | UNIX domain socket server     |
| `W`/`w` | Γ       |       | tuple/map horizontal join     |
| `X`     | P       |       | prefix for OT integral        |
| `Y`     | P       |       | prefix for OT derivative      |
| `Z`     | P       |       | prefix for OT                 |

| Symbol | Prefix? | Full? | Description                     |
|--------|---------|-------|---------------------------------|
| `@`    | P       |       | prefix for sqlite               |
| `#`    | P       |       | prefix for other DBs            |
| `~`    | P       |       | prefix for delay and rate-limit |
| `<`    | P       |       | prefix for unboxing             |
| `>`    | P       |       | prefix for boxing               |
| `?`    | P       |       | prefix for lossy queues         |
| `/`    | P       |       | platform-specific namespace     |
| `!`    |         |       | evaluate a Γ to a static ψ      |
| `+`    |         |       | τ-group append                  |
| `&`    |         |       |                                 |
| `=`    |         |       |                                 |
| `-`    |         |       |                                 |
| `*`    |         |       |                                 |
| `\``   |         |       |                                 |
| `'`    |         |       | dynamic transpose (cross-mix)   |
| `"`    |         |       |                                 |
| `←`    |         | n v   | define variable                 |
| `$`    |         | n     | refer to variable               |
| `.`    |         |       | Ξ subscript                     |
| `,`    |         |       | ξ bundle (collect into Ξ)       |
| `%`    |         |       | Γ union (eager interleave)      |
| `^`    |         |       | τ-group prepend                 |
| `_`    |         |       | null port                       |
| `:`    |         |       | omni-blocking broadcast         |
| `;`    |         |       | non-blocking side tap           |
