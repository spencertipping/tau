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


## π toplevel table (`π`)
| Symbol | Syntax | Description       |
|--------|--------|-------------------|
| `←`    | n v    | define variable   |
| `$`    | n      | refer to variable |


## Lens symbol table (`L`)
| Symbol   | Description                 |
|----------|-----------------------------|
| `L`      | confirm that this is a lens |
| `[`/`]`  | Lens grouping               |
| `(`      | `tuple-wrap-left`           |
| `)`      | `tuple-wrap-right`          |
| `^`      | `tuple-cons-left`           |
| `+`      | `tuple-cons-right`          |
| `a`..`z` | `tuple-element`             |
| `.k`     | `map-key(k)`                |

**NOTE:** lenses are composable by concatenation: `a.foo` refers to the `foo` key of the first element of a tuple; `ab` is `η[0][1]`. Whitespace is required to refer to a tuple element within a map key: `.foo a`.


## Γ topology symbol table (`Γ`)
| Symbol  | Syntax     | Description                       |
|---------|------------|-----------------------------------|
| `[`/`]` | Γ\*        | Γ grouping                        |
| `(`/`)` | L? Γ\*     | dynamic half-duplex demultiplexer |
| `{`/`}` | L? (k Γ)\* | static full-duplex demultiplexer  |
| \`A     |            | authentication prefix             |
| \`D     |            | decryption prefix                 |
| \`E     |            | encryption prefix                 |
| \`P     | Γ          | fork to local process             |
| \`X     |            | OT integral prefix                |
| \`Y     |            | OT derivative prefix              |
| \`Z     |            | OT structure prefix               |
| `\|`    | Γp         | modify processor to right-cap     |
| `\\`    | Γp         | modify processor to backward      |
| `.`     |            | Ξ subscript                       |
| `,`     |            | ξ bundle (collect into Ξ)         |
| `:`     |            | omni-blocking broadcast           |
| `;`     |            | non-blocking side tap             |
|         | Γp         | implicit fallover to half-duplex  |


## Γ half-duplex table (`Γp`)
| Symbol   | Notes | Description                      |
|----------|-------|----------------------------------|
| `a`      | R     | reserved for lens disambiguation |
| `b`      | R     |                                  |
| `c`      | R     |                                  |
| `d`      | R     |                                  |
| `e`      | R     |                                  |
| `f`      |       | tuple field transform            |
| `g`      |       | τ-grouped sort                   |
| `h`      |       | HTTP/S+WS client                 |
| `i`      |       | η identity out                   |
| `j`      | L? Γ  | τ-grouped sorted join            |
| `k`      |       | τ after each item                |
| `l`      |       |                                  |
| `m`      |       | π 1:1 map                        |
| `n`/`N`  |       | numeric ι/ϊ                      |
| `o`      |       |                                  |
| `p`      | π     | π program                        |
| `q`      |       |                                  |
| `r`/`R`  | π     | π row selector (+ lossy queues)  |
| `s`      | P     | prefix for shell commands        |
| `t`      |       | TCP client                       |
| `u`      |       | UDP client                       |
| `v`      |       | UNIX domain socket client        |
| `w`/`W`  | Γ     | tuple/map horizontal join        |
| `x`      |       | exchange tuple fields            |
| `y`      |       |                                  |
| `z`      | P     | prefix for compression           |
| `A`      |       |                                  |
| `B`      | P     | prefix for bounded buffers       |
| `C`      |       |                                  |
| `D`      |       |                                  |
| `E`      |       |                                  |
| `F`      | P     | prefix for formatted IO          |
| `G`      |       | stateful gate                    |
| `H`      | Γ     | HTTP+WS server                   |
| `I`      |       | η repeated out                   |
| `J`      | L? Γ  | τ-grouped unordered join         |
| `K`      |       | τ repeater                       |
| `L`      | R     | reserved for lens prefix         |
| `M`      |       | monitor                          |
| `N`/`n`  |       | numeric ϊ/ι                      |
| `O`      |       |                                  |
| `P`      |       |                                  |
| `Q`      |       | prefix for queues                |
| `R`/`r`  | π     | π row selector                   |
| `S`      | P     | prefix for horizontal scaling    |
| `T`      | Γ     | TCP server                       |
| `U`      | Γ     | UDP server                       |
| `V`      | Γ     | UNIX domain socket server        |
| `W`/`w`  | Γ     | tuple/map horizontal join        |
| `X`      |       |                                  |
| `Y`      |       |                                  |
| `Z`      |       |                                  |
| `#`      |       |                                  |
| `@`      | P     | prefix for DBs                   |
| `~`      | P     | prefix for delay and rate-limit  |
| `<`      | P     | prefix for unboxing              |
| `>`      | P     | prefix for boxing                |
| `?`      |       |                                  |
| `/`      | P     | platform-specific namespace      |
| `!`      |       | evaluate a Γ to a static ψ       |
| `+`      |       | τ-group append                   |
| `^`      |       | τ-group prepend                  |
| `%`      |       | Γ union (eager interleave)       |
| `:+`     |       | multi-way append                 |
| `:^`     |       | multi-way prepend                |
| `:%`     |       | multi-way interleave             |
| `&`      |       |                                  |
| `=`      |       |                                  |
| `-`      |       |                                  |
| `*`      |       |                                  |
| backtick | R     | reserved for prefixing           |
| `'`      |       | dynamic transpose (cross-mix)    |
| `"`      |       |                                  |
| `_`      |       | null port                        |
