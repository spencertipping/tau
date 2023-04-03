# σ: the τ standard library
σ defines common utilities that make τ ergonomic and useful. This includes [Γ](Gamma.md) components and [φ](phi.md) parsers. The `σ` namespace doesn't export `τ` by default. Most programs don't need to import `τ`, as `σ` provides entry points for common operations.


## Complex topologies
+ [Multiplexers and duplex connections](sigma-multiplex.md)


## Toplevel entities
The σ parser is split into context-specific branches, each of which has its own symbol table:

+ [Γ](Gamma.md)
+ [Ψ](Psi.md)
+ [π](pi.md)


## Parse tables
First an overall map of which characters are mapped to which tables. Numbers represent Ψ levels, `S` represents syntax (comments/whitespace). π is listed separately below.

```

sym  ABCDEFGHIJKLMNOPQRSTUVWXYZ abcdefghijklmnopqrstuvwxyz
Γ/Ψ   2 ΓΓ   0   22 Γ2200002444S     222022 20 2 222222222
π    ηηηηηη   m    m           S  m           m   d    vvv

sym  !@#$%^&*()-=_+{}[]\|`~<>,.:;"'?/
Γ/Ψ   4SΓ22Γ      2ΓΓΓΓΓΓ 222  Γ  4 4
π    mLS ddddLLdd d  LL d mdd    LL L
```


### Γ table
| Symbol  | Syntax  | Description                       |
|---------|---------|-----------------------------------|
| `:`     | n v     | define variable                   |
| `$`     | n       | refer to variable                 |
| `&`     | n       | refer to τ server socket          |
| `[`/`]` | Γ\*     | Γ grouping                        |
| `(`/`)` | Γ\*     | dynamic half-duplex demultiplexer |
| `{`/`}` | (k Γ)\* | static full-duplex demultiplexer  |
| `A`     |         | authentication prefix             |
| `D`     |         | decryption prefix                 |
| `E`     |         | encryption prefix                 |
| `P`     | Γ       | fork to local process             |
| `\|`    | Ψ[012]  | modify processor to right-cap     |
| `\\`    | Ψ[012]  | modify processor to backward      |


### Ψ₀ table
| Symbol | Notes | Description               |
|--------|-------|---------------------------|
| `i`    |       | η identity out            |
| `I`    |       | η repeated out            |
| `n`    |       | ι                         |
| `S`    | Γ     | SSH server                |
| `T`    | Γ     | TCP server                |
| `U`    | Γ     | UDP server                |
| `V`    | Γ     | UNIX domain socket server |


### Ψ₁ table
| Symbol   | Notes | Description                     |
|----------|-------|---------------------------------|


### Ψ₂ half-duplex table
| Symbol  | Notes | Description                      |
|---------|-------|----------------------------------|
| `f`     |       | field transform                  |
| `g`     |       | τ-grouped sort                   |
| `h`     |       | HTTP/S+WS client                 |
| `j`     | L? Γ  | τ-grouped sorted join            |
| `k`     |       | τ after each item                |
| `m`     |       | π 1:1 map                        |
| `p`     | π     | π program                        |
| `r`/`R` | π     | π row selector + row drop macros |
| `s`     | P     | SSH client                       |
| `t`     |       | TCP client                       |
| `u`     |       | UDP client                       |
| `v`     |       | UNIX domain socket client        |
| `w`/`W` | Γ     | tuple/map horizontal join        |
| `x`     |       | prefix for shell execution       |
| `y`     | P     | prefix for Python interop        |
| `z`     | P     | prefix for compression           |
| `B`     | P     | prefix for bounded buffers       |
| `H`     | Γ     | HTTP+WS server                   |
| `J`     | L? Γ  | τ-grouped unordered join         |
| `M`     |       | monitor                          |
| `N`     |       | numeric ϊ                        |
| `P`     | P     | prefix for parallelism           |
| `Q`     |       | prefix for queues                |
| `<`     | P     | prefix for unboxing              |
| `>`     | P     | prefix for boxing                |
| `~`     | P     | prefix for delay and rate-limit  |
| `+`     | Γ     | τ-group append                   |
| `^`     | Γ     | τ-group prepend                  |
| `%`     | Γ     | Γ union (eager interleave)       |


### Ψ₄ full-duplex table
| Symbol | Notes | Description                   |
|--------|-------|-------------------------------|
| `'`    |       | dynamic transpose (cross-mix) |
| `@`    | P     | prefix for DBs                |
| `/`    | P     | platform-specific namespace   |
| `//`   | P     | synthetic namespace           |
| `X`    |       | OT integral prefix            |
| `Y`    |       | OT derivative prefix          |
| `Z`    |       | OT structure prefix           |


## π tables
### Input accessors (`η`)
| Symbol | Description |
|--------|-------------|
| `A`    | `η[0]`      |
| `B`    | `η[1]`      |
| `C`    | `η[2]`      |
| `D`    | `η[3]`      |
| `E`    | `η[4]`      |
| `F`    | `η[5]`      |
| `G`    | `η[6]`      |
| `H`    | `η[7]`      |


### Monadic functions (`m`)
| Symbol | Description               |
|--------|---------------------------|
| `c`    | count by unsorted element |
| `o`    | ascending sort            |
| `O`    | descending sort           |
| `J^`   | JSON decode               |
| `J`    | JSON encode               |
| `!`    | logical negate            |
| `~`    | bitwise negate            |


### Literals (`L`)
| Symbol   | Description                          |
|----------|--------------------------------------|
| `/`      | regex                                |
| `0`..`9` | numeric literal                      |
| `'`      | non-whitespace string literal        |
| `".."`   | general string literal               |
| `()`     | subexpression grouping               |
| `[]`     | sub-η                                |
| `@`      | symbol prefix, e.g. `@t`, `@f`, `@n` |


### Variables (`v`)
| Symbol | Description           |
|--------|-----------------------|
| `x`    | pre-reserved variable |
| `y`    | pre-reserved variable |
| `z`    | pre-reserved variable |


### Dyadic functions (`d`)
| Symbol | Description           |
|--------|-----------------------|
| `s`    | substr/subarray/slice |
| `+`    | add/concatenate/union |
| `-`    | sub                   |
| `*`    | mul/intersect         |
| `%`    | mod                   |
| `//`   | div                   |
| `<`    | compare               |
| `>`    | compare               |
| `=`    | compare               |
| `&`    | bitwise and           |
| `\|`   | bitwise or            |
| `^`    | bitwise xor           |
