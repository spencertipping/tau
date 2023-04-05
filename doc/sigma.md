# σ: the τ standard library
σ defines common utilities that make τ ergonomic and useful. This includes [Γ](Gamma.md) components and [φ](phi.md) parsers. The `σ` namespace doesn't export `τ` by default. Most programs don't need to import `τ`, as `σ` provides entry points for common operations.


## Complex topologies
+ [Multiplexers and duplex connections](sigma-multiplex.md)


## Toplevel entities
The σ parser is split into context-specific branches, each of which has its own symbol table:

+ [Γ](Gamma.md)
+ [Ψ](Psi.md)
+ [π](pi.md)

You can think about it like this: Γ transforms _cables_ (many streams), Ψ transforms _streams_ (vertical), and π transforms _rows_ (horizontal). Some Ψs accept π expressions to perform some horizontal transformation before or after manipulating the rows.


## Parse tables
First an overall map of which characters are mapped to which tables. Numbers represent Ψ levels, `S` represents syntax (comments/whitespace). π is listed separately below.

```
sym  ABCDEFGHIJKLMNOPQRSTUVWXYZ abcdefghijklmnopqrstuvwxyz
Γ/Ψ   2 ΓΓ  00   22 Γ2200002444S      22022  0 2 222222222
π    ηηηηηm   m   ηm       d   SL m dmL    d mm ddd   dv

sym  !@#$%^&*()-=_+{}[]\|`~<>,.:;"'?/
Γ/Ψ   4SΓ22Γ      2ΓΓΓΓΓΓ 222  Γ  4 4
π    mLSvddddLLdd dLLLL d mddddddLLSL
```

**NOTE:** some punctuation is considered to be syntax (e.g. `?`) because it appears within other operators and would create ambiguous parsers if it were allowed to appear separately.


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
| `H`    | Γ     | HTTP+WS server            |
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
| `g`     | π     | τ-grouped sort by π expression   |
| `h`     |       | HTTP/S+WS client                 |
| `j`     | L? Γ  | τ-grouped sorted join            |
| `k`     |       | τ after each item                |
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
| `J`     | L? Γ  | τ-grouped unordered join         |
| `M`     |       | monitor                          |
| `N`     |       | numeric ϊ                        |
| `P`     | P     | prefix for parallelism           |
| `Q`     |       | prefix for queues/buffers        |
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
π expressions are parsed with prefix monadic and infix dyadic ops, just like APL. Unlike APL, however, we can't infer operator arity; expressions may be juxtaposed to form adjacent tuple/map elements, so we rely on operators to set our expectations about whether the next expression is part of this one or the beginning of a new element.


### Input accessors (`η`)
| Symbol | Description                 |
|--------|-----------------------------|
| `A`    | `η[0]`                      |
| `B`    | `η[1]`                      |
| `C`    | `η[2]`                      |
| `D`    | `η[3]`                      |
| `E`    | `η[4]`                      |
| `F`    | `η[5]`                      |
| `G`    | `η[6]`                      |
| `H`    | `η[7]`                      |
| `N`    | `\|η\|`: number of elements |

Note that due to the structure of η, `B` and `ni`'s `B.` are comparable here: you can always proceed rightward to the end of the input record.


### Literals (`L`)
| Symbol   | Description            |
|----------|------------------------|
| `ne`     | exponential constant   |
| `nf`     | φ numeric constant     |
| `np`     | π numeric constant     |
| `nt`     | τ numeric constant     |
| `ga`     | α signal               |
| `gk`     | κ signal               |
| `gi`     | ι signal               |
| `gt`     | τ signal               |
| `gw`     | ω signal               |
| `at`     | `true` atom            |
| `af`     | `false` atom           |
| `an`     | `null` atom            |
| `/`      | regex                  |
| `0`..`9` | numeric literal        |
| `'`      | name literal           |
| `".."`   | string literal         |
| `[]`     | subexpression grouping |
| `()`     | sub-η as tuple         |
| `{}`     | sub-η as map           |


### Variables (`v`)
| Symbol | Description           |
|--------|-----------------------|
| `$`    | variable prefix       |
| `x`    | pre-reserved variable |


### Monadic functions (`m`)
| Symbol | Description               |
|--------|---------------------------|
| `c`    | count by unsorted element |
| `f`    | floor                     |
| `F`    | ceiling                   |
| `n`    | length of string/vector   |
| `o`    | ascending sort            |
| `O`    | descending sort           |
| `J^`   | JSON decode               |
| `J`    | JSON encode               |
| `!`    | logical negate            |
| `~`    | bitwise negate            |
| `I?`   | check for integer type    |
| `F?`   | check for float type      |
| `N?`   | check for numeric type    |
| `S?`   | check for string type     |
| `A?`   | check for atom type       |
| `G?`   | check for signal type     |
| `V?`   | check for vector type     |
| `Y?`   | check for η type          |
| `I<`   | convert to int            |
| `F<`   | convert to float          |
| `S<`   | convert to string         |


### Dyadic functions (`d`)
| Symbol   | Description           |
|----------|-----------------------|
| `e`      | exp                   |
| `r`      | root                  |
| `l`      | log                   |
| `q`      | quantize              |
| `s`      | substr/subarray/slice |
| `w`      | tuple join natural    |
| `W`      | tuple join inverted   |
| `+`      | add/concatenate/union |
| `+[...]` | union-with            |
| `-`      | sub/difference        |
| `*`      | mul/intersect         |
| `*[...]` | intersect-with        |
| `%`      | mod/regex apply       |
| `//`     | div                   |
| `+/`     | sum of many           |
| `*/`     | product of many       |
| `<`      | compare               |
| `>`      | compare               |
| `=`      | compare               |
| `&`      | bitwise and           |
| `\|`     | bitwise or            |
| `^`      | bitwise xor           |
| `.`      | map element access    |
| `,`      | tuple element access  |
| `:`      | bind variable         |
| `;`      | sequence (return RHS) |
