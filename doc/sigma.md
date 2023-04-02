# σ: the τ standard library
σ defines common utilities that make τ ergonomic and useful. This includes [Γ](Gamma.md) components and [φ](phi.md) parsers. The `σ` namespace doesn't include `τ` by default. Most programs don't need to import `τ`, as `σ` provides entry points for common operations.

σ is designed for the program-authoring use case, meaning that you're dropped into a [π](pi.md) interpreter that is preloaded with σ bindings. The π program should evaluate to a Γ that is applied to an empty [Ξ](Xi.md) to form the initial steady state.

**TODO:** rewrite definitions below


## Complex topologies
+ [Multiplexers and duplex connections](sigma-multiplex.md)


## π toplevel table (`π`)
| Symbol | Syntax | Description              |
|--------|--------|--------------------------|
| `:`    | n v    | define variable          |
| `$`    | n      | refer to variable        |
| `&`    | n      | refer to τ server socket |


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
| `\|`    | Ψ2         | modify processor to right-cap     |
| `\\`    | Ψ2         | modify processor to backward      |
| `.`     |            | Ξ subscript                       |
| `,`     |            | ξ bundle (collect into Ξ)         |
| `!`     |            | evaluate a Γ to a static ψ        |
| `;`     |            | non-blocking side tap             |
| `:*`    |            | omni-blocking broadcast           |
| `:+`    |            | multi-way append                  |
| `:^`    |            | multi-way prepend                 |
| `:%`    |            | multi-way interleave              |
|         | Ψ2         | implicit fallover to half-duplex  |


## Γ half-duplex table (`Ψ2`)
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
| `x`      |       |                                  |
| `y`      | P     | prefix for Python interop        |
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
| `P`      | P     | prefix for parallelism           |
| `Q`      |       | prefix for queues                |
| `R`/`r`  | π     | π row selector                   |
| `S`      | Γ     | SSH server                       |
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
| `+`      |       | τ-group append                   |
| `^`      |       | τ-group prepend                  |
| `%`      |       | Γ union (eager interleave)       |
| `&`      |       |                                  |
| `=`      |       |                                  |
| `-`      |       |                                  |
| `*`      |       |                                  |
| backtick | R     | reserved for prefixing           |
| `'`      |       | dynamic transpose (cross-mix)    |
| `"`      |       |                                  |
| `_`      |       | null port                        |
