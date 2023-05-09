# σ/π stdlib
See [π](pi.md) for an overview of the language structure. This page describes the functions/operators that σ defines for π.

**FIXME:** rewrite a lot of this: we now have plural operators and more domains (string, numeric, etc)


## Operator classes
+ Numeric monads (`sin`, `cos`, `~`)
+ Numeric dyads (`+`, `//`, `|`)
+ String monads (`lc`, `uc`)
+ String dyads (`++`, `=~`, `find`)
+ Sequence monads (`n`, `o`, `O`, `uniq`, `freqs`)
+ Sequence dyads (`++`, `-`, `union`, `intersect`, `+[]`, `*[]`)


## Parse map
```
sym  ABCDEFGHIJKLMNOPQRSTUVWXYZ abcdefghijklmnopqrstuvwxyz
π    ηηηηηm   m   ηm         η SL m dmL    d mm ddt     η
R?        R                       R ?R     ?    ??

sym  !@#$%^&*()-=_+{}[]\|`~<>,.:;"'?/
π    mLSLddddLLdd dLLLL dSmddddddLLtL
```

"Status" refers to implementation status and is one of the following:

+ `N`: not implemented
+ `R`: rename is planned
+ `E`: experimental
+ `P`: partially implemented
+ `I`: implemented but behavior may change
+ `S`: stable


## Examples
```bash
$ diff <(bin/sigma 'n10M?F') <(seq 0 9)
$ diff <(bin/sigma 'n10p[y+1]M?F') <(seq 10)
$ diff <(bin/sigma ':foo [n10p[y+1]] :bar [n20] $foo M?F') <(seq 10)
```


## Input accessors (`η`)
| Symbol | Status | Description |
|--------|--------|-------------|
| `y`    | I      | current η   |


## Literals (`L`)
**FIXME:** claiming `at*`, `af*`, and other such literal prefixes is pretty aggressive; let's namespace these values better if possible

| Symbol   | Status | Description                  |
|----------|--------|------------------------------|
| `ne`     | N      | exponential constant         |
| `nf`     | N      | φ numeric constant           |
| `np`     | N      | π numeric constant           |
| `nt`     | N      | τ numeric constant           |
| `ga`     | N      | α signal                     |
| `gk`     | N      | κ signal                     |
| `gi`     | N      | ι signal                     |
| `gt`     | N      | τ signal                     |
| `gw`     | N      | ω signal                     |
| `at`     | N      | `true` atom                  |
| `af`     | N      | `false` atom                 |
| `an`     | N      | `null` atom                  |
| `/`      | N      | regex                        |
| `0`..`9` | I      | numeric literal              |
| `'`      | I      | name literal                 |
| `".."`   | I      | string literal               |
| `[]`     | I      | subexpression grouping       |
| `()`     | I      | sub-η as tuple               |
| `{}`     | N      | quote expression as function |
| `$`      | N      | variable reference           |


## Monadic functions (`m`)
| Symbol | Status | Description                   |
|--------|--------|-------------------------------|
| `c`    | R      | count unsorted elements       |
| `f`    | R      | floor                         |
| `F`    | R      | ceiling                       |
| `n`    | N      | length of string/vector/tuple |
| `o`    | N      | ascending sort                |
| `O`    | N      | descending sort               |
| `J^`   | N      | JSON decode                   |
| `J`    | N      | JSON encode                   |
| `!`    | I      | logical negate                |
| `~`    | I      | bitwise negate                |
| `?I`   | N      | check for integer type        |
| `?F`   | N      | check for float type          |
| `?N`   | N      | check for numeric type        |
| `?S`   | N      | check for string type         |
| `?A`   | N      | check for atom type           |
| `?G`   | N      | check for signal type         |
| `?V`   | N      | check for vector type         |
| `?Y`   | N      | check for η type              |
| `>I`   | N      | convert to int                |
| `>F`   | N      | convert to float              |
| `>S`   | N      | convert to string             |
| `+/`   | N      | sum of many                   |
| `*/`   | N      | product of many               |
| `+\\`  | N      | summing reduction (integral)  |
| `-\\`  | N      | delta reduction (derivative)  |
| `*\\`  | N      | product reduction             |


## Dyadic functions (`d`)
| Symbol   | Status | Arguments        | Description                     |
|----------|--------|------------------|---------------------------------|
| `e`      | R?     | power base=e     | exp                             |
| `l`      | R?     | number base=e    | log                             |
| `r`      | R?     | number root=2    | root                            |
| `q`      | R?     | number quantum=1 | quantize                        |
| `+`      | P      |                  | add/concatenate/union           |
| `+[...]` | N      |                  | union-with                      |
| `-`      | P      |                  | sub/difference                  |
| `*`      | P      |                  | mul/intersect                   |
| `*[...]` | N      |                  | intersect-with                  |
| `%`      | P      |                  | mod/regex apply                 |
| `//`     | I      |                  | div                             |
| `>>`     | I      | n bits           | right shift                     |
| `<<`     | I      | n bits           | left shift                      |
| `<`      | I      |                  | compare                         |
| `<=`     | I      |                  | compare                         |
| `>`      | I      |                  | compare                         |
| `>=`     | I      |                  | compare                         |
| `=`      | I      |                  | compare                         |
| `!=`     | I      |                  | compare                         |
| `<>`     | I      |                  | three-way compare (-1, 0, 1, 2) |
| `&`      | I      |                  | bitwise and                     |
| `\|`     | I      |                  | bitwise or                      |
| `^`      | I      |                  | bitwise xor                     |
| `&&`     | N      |                  | logical and                     |
| `\|\|`   | N      |                  | logical or`                     |
| `.`      | N      | map key          | map element access              |
| `:`      | N      | name-str value   | bind variable                   |
| `,`      | N      | lhs rhs          | write (return RHS)              |
| `;`      | N      | lhs rhs          | sequence (return RHS)           |


## Ternary functions (`t`)
Ternary functions are placed between the first and second arguments; the third is parsed as an immediate continuation. See [π spec](pi.md) for details.

| Symbol | Status | Arguments      | Description           |
|--------|--------|----------------|-----------------------|
| `?`    | I      | b t e          | conditional           |
| `s`    | N      | xs lower upper | substr/subarray/slice |
