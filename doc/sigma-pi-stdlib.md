# σ/π stdlib
See [π](pi.md) for an overview of the language structure. This page describes the functions/operators that σ defines for π. First the parse map:

```
sym  ABCDEFGHIJKLMNOPQRSTUVWXYZ abcdefghijklmnopqrstuvwxyz
π    ηηηηηm   m   ηm           SL m dmL    d mm ddt    vv

sym  !@#$%^&*()-=_+{}[]\|`~<>,.:;"'?/
π    mLSvddddLLdd dLLLL d mddddddLLtL
```


## Examples
**TODO**


## Input accessors (`η`)
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


## Literals (`L`)
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


## Variables (`v`)
| Symbol | Description           |
|--------|-----------------------|
| `$`    | variable prefix       |
| `x`    | pre-reserved variable |
| `y`    | pre-reserved variable |


## Monadic functions (`m`)
| Symbol | Description                   |
|--------|-------------------------------|
| `c`    | count unsorted elements       |
| `f`    | floor                         |
| `F`    | ceiling                       |
| `n`    | length of string/vector/tuple |
| `o`    | ascending sort                |
| `O`    | descending sort               |
| `J^`   | JSON decode                   |
| `J`    | JSON encode                   |
| `!`    | logical negate                |
| `~`    | bitwise negate                |
| `?I`   | check for integer type        |
| `?F`   | check for float type          |
| `?N`   | check for numeric type        |
| `?S`   | check for string type         |
| `?A`   | check for atom type           |
| `?G`   | check for signal type         |
| `?V`   | check for vector type         |
| `?Y`   | check for η type              |
| `>I`   | convert to int                |
| `>F`   | convert to float              |
| `>S`   | convert to string             |
| `+/`   | sum of many                   |
| `*/`   | product of many               |
| `+\\`  | summing reduction (integral)  |
| `-\\`  | delta reduction (derivative)  |
| `*\\`  | product reduction             |


## Dyadic functions (`d`)
| Symbol   | Arguments        | Description                     |
|----------|------------------|---------------------------------|
| `e`      | power base=e     | exp                             |
| `l`      | number base=e    | log                             |
| `r`      | number root=2    | root                            |
| `q`      | number quantum=1 | quantize                        |
| `+`      |                  | add/concatenate/union           |
| `+[...]` |                  | union-with                      |
| `-`      |                  | sub/difference                  |
| `*`      |                  | mul/intersect                   |
| `*[...]` |                  | intersect-with                  |
| `%`      |                  | mod/regex apply                 |
| `//`     |                  | div                             |
| `<`      |                  | compare                         |
| `<=`     |                  | compare                         |
| `>`      |                  | compare                         |
| `>=`     |                  | compare                         |
| `=`      |                  | compare                         |
| `!=`     |                  | compare                         |
| `<>`     |                  | three-way compare (-1, 0, 1, 2) |
| `&`      |                  | bitwise and                     |
| `\|`     |                  | bitwise or                      |
| `^`      |                  | bitwise xor                     |
| `&&`     |                  | logical and                     |
| `\|\|`   |                  | logical or`                     |
| `.`      | map key          | map element access              |
| `@`      | tuple idx        | tuple element access            |
| `:`      | name-str value   | bind variable                   |
| `,`      | lhs rhs          | write (return RHS)              |
| `;`      | lhs rhs          | sequence (return RHS)           |


## Ternary functions (`t`)
Ternary functions are placed between the first and second arguments; the third is parsed as an immediate continuation. See [π spec](pi.md) for details.

| Symbol | Arguments      | Description           |
|--------|----------------|-----------------------|
| `?`    | b t e          | conditional           |
| `s`    | xs lower upper | substr/subarray/slice |
