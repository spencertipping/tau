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
Monadic, dyadic, and triadic lambda expressions can be written using `{]`, `{}`, and `{)` groups respectively.

```
a ::= ... | '{' p ']' | '{' p '}' | '{' p ')'
```


## V1
We can get by without most of the `ms` and `ds` operators for now, and also without functions. So we just need to implement a more basic grammar:

```
a ::= lit | '[' s ']'? | '(' p ')'?
s ::= a | m a | a d s | a t s s
p ::= (a | m a | a d s | a t s s)*
```


## Examples
```bash
$ bin/pi-phi '3+4' '"foo"' '3+2*5' '
# this is a test
[123+100] < 456  # this was a test' \
'(10 (1 + 2 3) 4 5)' \
'(10 @(11 12 13) 14 15)'
7
"foo"
13
true
(10 (3 3) 4 5)
(10 11 12 13 14 15)
$ bin/pi-phi '"foo" + "bar"'
"foobar"
```

Testing some builtins:

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
