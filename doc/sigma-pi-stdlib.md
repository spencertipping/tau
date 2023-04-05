# σ/π stdlib
π expressions are parsed with prefix monadic and infix dyadic ops, just like APL. Unlike APL, however, we can't infer operator arity; expressions may be juxtaposed to form adjacent tuple/map elements, so we rely on operators to set our expectations about whether the next expression is part of this one or the beginning of a new element.

Monadic and dyadic operators may include dyadic operator characters after their first position, since these can normally be parsed only after a value. For example, `S<` is a valid monadic operator because `S` cannot be interpreted as a value, so `<` must not be a dyadic operator in this context.

Like APL, operators apply strictly right to left: `!3>4` is evaluated as `!(3>4)`. We don't have operator precedence.

```
sym  ABCDEFGHIJKLMNOPQRSTUVWXYZ abcdefghijklmnopqrstuvwxyz
π    ηηηηηm   m   ηm           SL m dmL    d mm ddd    vv

sym  !@#$%^&*()-=_+{}[]\|`~<>,.:;"'?/
π    mLSvddddLLdd dLLLL d mddddddLLSL
```

**NOTE:** some punctuation is considered to be syntax (e.g. `?`) because it appears within other operators and would create ambiguous parsers if it were allowed to appear separately.


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
| `+/`   | sum of many               |
| `*/`   | product of many           |


## Dyadic functions (`d`)
| Symbol   | Arguments        | Description           |
|----------|------------------|-----------------------|
| `e`      | power base=e     | exp                   |
| `l`      | number base=e    | log                   |
| `r`      | number root=2    | root                  |
| `q`      | number quantum=1 | quantize              |
| `s`      |                  | substr/subarray/slice |
| `+`      |                  | add/concatenate/union |
| `+[...]` |                  | union-with            |
| `-`      |                  | sub/difference        |
| `*`      |                  | mul/intersect         |
| `*[...]` |                  | intersect-with        |
| `%`      |                  | mod/regex apply       |
| `//`     |                  | div                   |
| `<`      |                  | compare               |
| `>`      |                  | compare               |
| `=`      |                  | compare               |
| `&`      |                  | bitwise and           |
| `\|`     |                  | bitwise or            |
| `^`      |                  | bitwise xor           |
| `.`      | map key          | map element access    |
| `,`      | tuple idx        | tuple element access  |
| `:`      | name-str value   | bind variable         |
| `;`      | lhs rhs          | sequence (return RHS) |
