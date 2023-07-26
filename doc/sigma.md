# σ: the τ standard library
σ defines common utilities that make τ ergonomic and useful. This includes [Γ](Gamma.md) components and [φ](phi.md) parsers. The `σ` namespace doesn't export `τ` by default. Most programs don't need to import `τ`, as `σ` provides entry points for common operations.


## Sub-topics
+ [Multiplexers and duplex connections](sigma-multiplex.md)
+ [π standard library](sigma-pi-stdlib.md)
+ [σ search](sigma-search.md)
+ [σ containers](sigma-containers.md)


## Toplevel entities
The σ parser is split into context-specific branches, each of which has its own symbol table:

+ [Γ](Gamma.md)
+ [Ψ](Psi.md)
+ [π](pi.md)

You can think about it like this: Γ transforms Ξ _cables_ (many streams), Ψ transforms ξ _streams_ (vertical), and π transforms η _rows_ (horizontal). Some Ψs accept π expressions to perform some horizontal transformation before or after manipulating the rows.

π is somewhat involved and its standard library is documented [separately](sigma-pi-stdlib.md).


```bash
$ bin/sigma-fast 'n1p[@-("a" 1)("b" 2 3)("a" 0)("b" 1)(τ);`
                      @-("a" 1)("Z" 1)("0" 0)]
                    g M?>_'
"a" 0
"a" 1
"b" 1
"b" 2 3
τ
"0" 0
"Z" 1
"a" 1
τ
```


## Parse tables
First an overall map of which characters are mapped to which tables. Numbers represent Ψ levels, `S` represents syntax (comments/whitespace).

```
sym  ABCDEFGHIJKLMNOPQRSTUVWXYZ abcdefghijklmnopqrstuvwxyz
Γ/Ψ     ΓΓ  00   22 Γ2211112444S      22022  0 2 22222222

sym  !@#$%^&*()-=_+{}[]\|`~<>,.:;"'?/
Γ/Ψ   2SΓ22Γ2ΓΓ2ΓΓ2ΓΓΓΓΓΓ 2**2 Γ2 424
```


## TODO: GC everything below


### Γ table
| Symbol  | Syntax  | Description                               |
|---------|---------|-------------------------------------------|
| `:`     | n Γ     | define Γ variable                         |
| `$`     | n       | refer to Γ                                |
| `:/`    | n       | bind top ξ to `n`                         |
| `$/`    | n       | recall ξ                                  |
| `$$`    |         | swap top ξs                               |
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
| Symbol | Syntax | Description          |
|--------|--------|----------------------|
| `>F`   | fd     | write to FD          |
| `>_`   |        | consume data forever |


### Ψ₁ table
| Symbol | Notes | Description                    |
|--------|-------|--------------------------------|
| `<F`   | fd    | read from FD                   |
| `<_`   |       | wait forever, write nothing    |
| `i`    | π     | η identity out                 |
| `I`    | π     | η repeated out                 |
| `n`    |       | ι                              |
| `S`    | Γ     | SSH server                     |
| `T`    | Γ     | [TCP server](sigma-http-ws.md) |
| `U`    | Γ     | UDP server                     |
| `V`    | Γ     | UNIX domain socket server      |


### Ψ₂ half-duplex table
**TODO:** design sort/join operators that work together to produce streaming τ-cyclic parallel sort

| Symbol  | Notes | Description                                       |
|---------|-------|---------------------------------------------------|
| `g`     | π     | τ-grouped sort by π expression                    |
| `h`     |       | HTTP/S+WS client                                  |
| `j`     | Γ     | τ-grouped sorted join                             |
| `k`     |       | τ after each item                                 |
| `K`     |       | remove signals                                    |
| `p`     | π     | π program                                         |
| `pf`    | π     | π program with recursive futures                  |
| `r`/`R` | π     | π row selector                                    |
| `r^`    | int   | take/drop first N rows (per τ)                    |
| `r$`    | int   | take/drop last N rows (per τ)                     |
| `s`     | P     | SSH client                                        |
| `t`     |       | TCP client                                        |
| `u`     |       | UDP client                                        |
| `v`     |       | UNIX domain socket client                         |
| `w`/`W` | Γ     | tuple/map horizontal join                         |
| `x`     |       | prefix for shell execution                        |
| `y`     | P     | prefix for Python interop                         |
| `J`     | L? Γ  | τ-grouped unordered join                          |
| `M?`    |       | monitor, debug-print each value                   |
| `M??`   | tag   | monitor, debug-print each value with tag          |
| `N`     |       | numeric ϊ                                         |
| `P`     | P     | prefix for parallelism                            |
| `Q`     |       | prefix for queues/buffers                         |
| `@`     | P     | prefix for maps and sets                          |
| `<`     | P     | prefix for unboxing                               |
| `>`     | P     | prefix for boxing                                 |
| `~`     | P     | prefix for delay and rate-limit                   |
| `+`     | Γ     | τ-group append                                    |
| `^`     | Γ     | τ-group prepend                                   |
| `%`     | Γ     | Γ union (eager interleave)                        |
| `-`     |       | identity passthrough                              |
| `,`     |       | break stream                                      |
| `;`     | Γ     | splice Ψ4 to Ψ2                                   |
| `?`     | P     | [incremental search prefix](sigma-search.md)      |
| `*`     | P     | [replicated multiplex prefix](sigma-multiplex.md) |


### Ψ₄ full-duplex table
| Symbol | Notes | Description                        |
|--------|-------|------------------------------------|
| `'`    |       | dynamic transpose (cross-mix)      |
| `/`    | P     | platform-specific namespace        |
| `//`   | P     | synthetic namespace                |
| `H`    | Γ     | [HTTP+WS server](sigma-http-ws.md) |
| `X`    |       | OT integral prefix                 |
| `Y`    |       | OT derivative prefix               |
| `Z`    |       | OT structure prefix                |
