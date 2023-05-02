# π scripting
π is a series of [φ](phi.md) parsers that allow [η](eta.md)-transforming functions to be defined from string inputs, ultimately making it possible to write many τ programs without compiling code.

+ [π interpreter](pi-int.md)
+ [σ/π stdlib](sigma-pi-stdlib.md)


## Expressions
π has two parse contexts: _singular_ and _plural_, each of which has slightly different behavior around operators and adjacent values.

+ Atoms: `a`
+ Monadic operators: `m x`
+ Dyadic operators: `x d y`
+ Triadic operators: `x t y z`

**TODO:** rewrite everything below


## π expression structure
π expressions are parsed with prefix monadic and infix dyadic ops, just like APL. Unlike APL, however, we can't infer operator arity; expressions may be juxtaposed to form adjacent tuple/map elements, so we rely on operators to set our expectations about whether the next expression is part of this one or the beginning of a new element.

Monadic and dyadic operators may include dyadic operator characters after their first position, since these can normally be parsed only after a value. For example, `S<` is a valid monadic operator because `S` cannot be interpreted as a value, so `<` must not be a dyadic operator in this context.

Like APL, operators apply strictly right to left: `!3>4` is evaluated as `!(3>4)`. We don't have operator precedence.

Unlike APL, multiple adjacent expressions are not parsed into a vector by default. `1 2 3` will result in an η with three separate integers. Operators apply within subexpressions: `1+2 3` is equivalent to `3 3`.


## Plurality
π, like Perl, has separate parsing contexts for single and plural values. This follows from η's "stream-of-values" structure, and is also inspired by the splice operators in Javascript, Ruby, and Python.


## Ternary expressions
π is built to parse multiple expressions independently, which means we can easily define a ternary operator without a delimiter. That is, we can define `?` without a corresponding `:` because the left and right subexpressions will be parsed adjacent to one another. This yields tighter packing:

```
x?3:y  // C++
x?3y   // π
```


## Examples
```bash
$ bin/pi-phi '3+4' '"foo"' '3+2*5' '
# this is a test
[123+100] < 456  # this was a test'
7
"foo"
13
true
```


## Scratch tests
```bash
$ bin/pi-eta
f(5) = 6
f("bar") = foobar
f(true) = fallthrough
a = 5
b = 5
template invocation: 5, 5
string branch: foo
[add1] x + y = 7
[add2] x + y = 7
1 || error = 1
0 || 3 = 3
i8s<0 2 0 4 0 10>
i16s<1 3 5>
i16s<2 4 10>
i64s<4 5 4 6 4 9>
i64s<5 6 9>
f64s<7 8 7 9 7 12>
f64s<8 9 12>
```
