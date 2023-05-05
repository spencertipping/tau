# π scripting
π is a series of [φ](phi.md) parsers that allow [η](eta.md)-transforming functions to be defined from string inputs, ultimately making it possible to write many τ programs without compiling code.

+ [π interpreter](pi-int.md)
+ [σ/π stdlib](sigma-pi-stdlib.md)


## Toplevel syntax
```
prog ::= (expr '`')* expr
expr ::= p
```

`p` is a plural expression, defined below.


## Expressions
π has two parse contexts: _singular_ and _plural_, each of which has slightly different behavior around operators and adjacent values. Here's the grammar:

```
a ::= lit | '[' s ']'? | '(' p ')'?
s ::= a | m a | ms p | a d s | a ds p | a t s s
p ::= (a | m a | a d s | a t s s)* (ms p | a ds p)?
```

Operators associate rightwards with no relative precedence, just like they do in APL.

Dyadic and triadic operators have no left-plural variants.

Some ambiguities are possible unless `a`, (`m` + `ms`), (`d` + `ds`), and `t` are all prefix-disjoint. This disjunction is not enforced, but σ is designed with it in mind.

APL requires parentheses around vector elements; that is, its plurality takes high precedence. π does not have this limitation because we assume that plurality extends rightwards until the current parse fails.


## Functions
**TODO:** allocate syntax, figure out how singular/plural works here, if it applies (probably `{}`, but how do we define arity? **switch the closing delimiter:** `]` for monadic, `}` for dyadic, and `)` for ternary)
