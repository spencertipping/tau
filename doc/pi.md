# π scripting
π is a series of [φ](phi.md) parsers that allow [η](eta.md)-transforming functions to be defined from string inputs, ultimately making it possible to write many τ programs without compiling code.

+ [π interpreter](pi-int.md)
+ [σ/π stdlib](sigma-pi-stdlib.md)


## π expression structure
π expressions are parsed with prefix monadic and infix dyadic ops, just like APL. Unlike APL, however, we can't infer operator arity; expressions may be juxtaposed to form adjacent tuple/map elements, so we rely on operators to set our expectations about whether the next expression is part of this one or the beginning of a new element.

Monadic and dyadic operators may include dyadic operator characters after their first position, since these can normally be parsed only after a value. For example, `S<` is a valid monadic operator because `S` cannot be interpreted as a value, so `<` must not be a dyadic operator in this context.

Like APL, operators apply strictly right to left: `!3>4` is evaluated as `!(3>4)`. We don't have operator precedence.

Unlike APL, multiple adjacent expressions are not parsed into a vector by default. `1 2 3` will result in an η with three separate integers. Operators apply within subexpressions: `1+2 3` is equivalent to `3 3`.


## Ternary expressions
π is built to parse multiple expressions independently, which means we can easily define a ternary operator without a delimiter. That is, we can define `?` without a corresponding `:` because the left and right subexpressions will be parsed adjacent to one another. This yields tighter packing:

```
x?3:y  // C++
x?3y   // π
```
