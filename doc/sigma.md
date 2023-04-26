# σ: the τ standard library
σ defines common utilities that make τ ergonomic and useful. This includes [Γ](Gamma.md) components and [φ](phi.md) parsers. The `σ` namespace doesn't export `τ` by default. Most programs don't need to import `τ`, as `σ` provides entry points for common operations.


## Sub-topics
+ [Multiplexers and duplex connections](sigma-multiplex.md)
+ [π standard library](sigma-pi-stdlib.md)


## Toplevel entities
The σ parser is split into context-specific branches, each of which has its own symbol table:

+ [Γ](Gamma.md)
+ [Ψ](Psi.md)
+ [π](pi.md)

You can think about it like this: Γ transforms _cables_ (many streams), Ψ transforms _streams_ (vertical), and π transforms _rows_ (horizontal). Some Ψs accept π expressions to perform some horizontal transformation before or after manipulating the rows.

π is somewhat involved and its standard library is documented [separately](sigma-pi-stdlib.md).


## Parse tables
First an overall map of which characters are mapped to which tables. Numbers represent Ψ levels, `S` represents syntax (comments/whitespace).

```
sym  ABCDEFGHIJKLMNOPQRSTUVWXYZ abcdefghijklmnopqrstuvwxyz
Γ/Ψ     ΓΓ0 00   22 Γ2200002444S      22022  0 2 22222222

sym  !@#$%^&*()-=_+{}[]\|`~<>,.:;"'?/
Γ/Ψ   4SΓ22Γ ΓΓ2ΓΓ2ΓΓΓΓΓΓ 222  Γ  4 4
Γ/Ψ                        10
```


### Γ table
| Symbol  | Syntax  | Description                               |
|---------|---------|-------------------------------------------|
| `:`     | n Γ     | define variable                           |
| `$`     | n       | refer to variable                         |
| `&`     | n       | refer to τ server socket                  |
| `[`/`]` | Γ\*     | Γ grouping                                |
| `(`/`)` | Γ\*     | dynamic half-duplex demultiplexer         |
| `{`/`}` | (k Γ)\* | static full-duplex demultiplexer          |
| `A`     |         | [authentication prefix](sigma-http-ws.md) |
| `D`     |         | decryption prefix                         |
| `E`     |         | encryption prefix                         |
| `P`     | Γ       | fork to local process                     |
| `\|`    | Ψ[012]  | modify processor to right-cap             |
| `\\`    | Ψ[012]  | modify processor to backward              |
| `:)`    | n Γ     | define tagged-union server                |
| `=`     |         | push new Ξ duplex                         |
| `_`     |         | drop Ξ duplex                             |


### Ψ₀ table
| Symbol | Notes | Description          |
|--------|-------|----------------------|
| `>F`   | fd    | write to FD          |
| `F`    |       | consume data forever |


### Ψ₁ table
| Symbol | Notes | Description                    |
|--------|-------|--------------------------------|
| `<F`   | fd    | read from FD                   |
| `f`    |       | wait forever, write nothing    |
| `i`    | π     | η identity out                 |
| `I`    | π     | η repeated out                 |
| `n`    |       | ι                              |
| `S`    | Γ     | SSH server                     |
| `T`    | Γ     | [TCP server](sigma-http-ws.md) |
| `U`    | Γ     | UDP server                     |
| `V`    | Γ     | UNIX domain socket server      |


### Ψ₂ half-duplex table
**TODO:** design sort/join operators that work together to produce streaming τ-cyclic parallel sort

| Symbol  | Notes | Description                              |
|---------|-------|------------------------------------------|
| `g`     | π     | τ-grouped sort by π expression           |
| `h`     |       | HTTP/S+WS client                         |
| `j`     | Γ     | τ-grouped sorted join                    |
| `k`     |       | τ after each item                        |
| `K`     |       | remove signals                           |
| `p`     | π     | π program                                |
| `r`/`R` | π     | π row selector                           |
| `r`     | int   | take/drop first N rows (per τ)           |
| `r+`    | int   | take/drop last N rows (per τ)            |
| `s`     | P     | SSH client                               |
| `t`     |       | TCP client                               |
| `u`     |       | UDP client                               |
| `v`     |       | UNIX domain socket client                |
| `w`/`W` | Γ     | tuple/map horizontal join                |
| `x`     |       | prefix for shell execution               |
| `y`     | P     | prefix for Python interop                |
| `J`     | L? Γ  | τ-grouped unordered join                 |
| `M?`    |       | monitor, debug-print each value          |
| `M??`   | tag   | monitor, debug-print each value with tag |
| `N`     |       | numeric ϊ                                |
| `P`     | P     | prefix for parallelism                   |
| `Q`     |       | prefix for queues/buffers                |
| `<`     | P     | prefix for unboxing                      |
| `>`     | P     | prefix for boxing                        |
| `~`     | P     | prefix for delay and rate-limit          |
| `+`     | Γ     | τ-group append                           |
| `^`     | Γ     | τ-group prepend                          |
| `%`     | Γ     | Γ union (eager interleave)               |
| `-`     |       | identity passthrough                     |


### Ψ₄ full-duplex table
| Symbol | Notes | Description                        |
|--------|-------|------------------------------------|
| `'`    |       | dynamic transpose (cross-mix)      |
| `@`    | P     | prefix for DBs                     |
| `/`    | P     | platform-specific namespace        |
| `//`   | P     | synthetic namespace                |
| `H`    | Γ     | [HTTP+WS server](sigma-http-ws.md) |
| `X`    |       | OT integral prefix                 |
| `Y`    |       | OT derivative prefix               |
| `Z`    |       | OT structure prefix                |
