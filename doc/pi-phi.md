# φ[π]
π is an APL-style infix expression language with singular and plural value contexts and no relative operator precedence. Operators typically bind right-to-left, so `x+y+1` is equivalent to `x+[y+1]`. `[]` are used to group expressions, `()` construct sub-η values such as tuples or maps.


## Singular and plural values
[η](eta.md) is a format that supports multiple values per row. π works with this by adopting a Perl-style hybrid singular/plural evaluation model: `3` is a single int in a stack slot, `3 4` are two ints that will be packed into one stack slot.

π views η values as combinations of _formed things_, each of which is a materialized η value stored on the heap somewhere. In the case of `3 4`, each is pushed separately and then they are fused into the resulting plural η record containing both (leaving the originals as garbage). If `$xs == 3 4`, then `$xs` is a single formed thing that refers to a heap entry. Here, `$xs` is a _singular_ value whereas `3 4` is _plural_ -- not because the values differ, but because they require different modes of construction.

This difference extends to operators: `3 4 + 1` is the same as `3 5`, whereas `$xs + 1` evaluates to `3+1`, or `4`. (Other operators may inspect more values within an η, but `+` is scalar and it just looks at the first one.)

So a plural value is just multiple singular values separated by commas and/or spaces. Here's the full grammar:

```
π ::= ... | (p '`')* p
p ::= ((s ','?)+ | pl) pr* ';'?
s ::= sl sr*

sl ::= '[' p ']' | '(' p ')' | ...
```

`π`, `sl`, `sr`, `pl`, and `pr` are all extensible parsers, some of which may refer back to `p` and `s` to parse subexpressions. For example, `?` is an `spost` whose parse continuation is `p ':' p`.

`p` expressions self-optimize when possible: if only one element is parsed, then no splicing is done. This prevents values from being unnecessarily copied when they are already on the heap.


## Greedy plurals
Perl and π both share the _greedy-plural_ problem: operators that accept a plural RHS will, by default, consume the rest of an expression. For example, suppose `+/` is a plural prefix operator, so it expects a plural context to its right. Then `[1 2 +/ 3 4 5]` results in `3 4 5` being consumed by `+/`.

π works around greedy plurals by terminating a plural parse using `;`. This means you can write `1 2 +/ 3 4; 5` to indicate that `5` belongs to the outer plural context, not to the `+/` argument.


## FFI and parser generation
The goal of π FFI is to derive as much interfacing information as possible from the function signature. For example, a lazy ternary operator should be definable like this:

```cpp
def_sr("?", [](πi &i, πSc<bool> &c,  // NOTE: πS<> is optional for clarity
                      πpc<π1> &t, φaL<':'>&, πpc<π1> &e)
  { return (c.x ? t.x(i) : e.x(i)).pop(); });
```

There's a lot going on here. First, each parameter may introduce a parser into the operator sequence. In this case we have three: `πp<π1>`, `φaL<":">`, and `πp<π1>`. `bool` is assumed to be present on the stack already since it's the left operand. This means our π function is a `πf<0>` and it will be parsed with this grammar:

```
'?' p ':' p
```


### Stack vs immediate operands
Why is `bool` pulled from the stack while `πp<π1>` is parsed? It has to do with the argument class, which is one of:

+ **Meta:** things like `πi&` and `πhr_<N>`, which have nothing to do with the stack nor parsing
+ **Stack:** things which are stored on the stack up front, and which can be η-decoded, e.g. `bool`, `πhr`, and other such types (`ηauto` covers these)
+ **Constant:** things which are not stored on the stack, e.g. `πs<π1>`, `φa...`, or `πP<T>`
+ **Immediate:** things which have an expression-mode annotation for parsing and are stored on the stack, e.g. `πsl<i64>`

Each immediate is inlined as a `πf<1>` that is prepended to the resulting function. This results in immediates being present above stack arguments. As a result, arguments must be ordered like this:

```
meta* stack* (const | immed)*
```

This works because immediate and stack arguments are popped right-to-left.


### Auto-constructing a `πf<N>`
`πf<N>` takes only a `πi&` and must construct the remaining function arguments. Parse-time arguments are stored as closure data and passed in as constants since they are runtime-invariant. Statics are calculated from the `πi&`, and runtime arguments are pulled from the stack and `ηauto` converted to C++ parameters.

All of this takes the form of a nested C++ function, the outer layer of which is transformed with `φauto` (receiving `Xs&&...` arguments), the inner layer transformed with `πvauto` (receiving `Ys&&...`), and calling the user function by splicing the arguments one by one to match the original ordering. This involves `πautoclass_<T>` to classify each argument into one of the categories above and handling it accordingly. See [pi-auto](../tau/pi-auto.hh) for implementation.
