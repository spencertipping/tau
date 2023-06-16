# φ[π]
π is an APL-style infix expression language with singular and plural value contexts and no relative operator precedence. It has some basic extensible grammar structures:

```
s ::= (s_atom | ss_pre s) s_post* | ps_pre p  ← fixed
p ::= (p_atom | sp_pre s | pp_pre p) p_post*  ← fixed
π ::= (s | p) ('`' (s | p))*                  ← fixed

p_atom ::= (s ','?)* | '[' p ']' | ...   ← extensible
s_atom ::= '(' p ')' | '[' s ']' | ...   ← extensible
```

`s` is a singular expression, `p` is a plural expression, and:

+ `ss_pre`: a prefix operator whose operand and result are both singular
+ `ps_pre`: a prefix operator whose operand is plural and result is singular
+ `sp_pre`: ... singular ... plural
+ `pp_pre`: ... plural ... plural
+ `s_post`: the parse continuation of a singular value, which may include infix operators
+ `p_post`: the parse continuation of a plural value, which may include infix operators

All of these are extensible dispatch parsers and `s_atom` and `p_atom` are extensible alternations.


## Extending the π grammar
A `πφ` instance provides several methods that allow you to define extensions:

+ `.def_sa(φ<π1>)`: define a new singular atom
+ `.def_pa(φ<π1>)`: define a new plural atom
+ `.def_ssp(name, fn)`: define a new `ss_pre` operator (see below)
+ `.def_psp(name, fn)`
+ `.def_spp(name, fn)`
+ `.def_ppp(name, fn)`
+ `.def_sp(name, fn)`: define a new `s_post` operator
+ `.def_pp(name, fn)`

Of these, `def_sa` and `def_pa` are the only methods that pass parsers directly through to the underlying grammar (albeit wrapping them in optional whitespace and comments). The others are transformed from functions that accept η-compatible arguments and produce η-compatible results.


## FFI and parser generation
The goal of π FFI is to derive as much interfacing information as possible from the function signature. For example, a lazy ternary operator should be definable like this:

```cpp
def_sp("?", [](πi &i, bool c, πse<π1> const &t, φaL<":">, πse<π1> const &e)
  { c ? t.x(i) : e.x(i);
    return i.pop(); });
```

There's a lot going on here. First, each parameter may introduce a parser into the operator sequence. In this case we have three: `πse<π1>`, `φaL<":">`, and `πse<π1>`. `bool` is assumed to be present on the stack already since it's the left operand. This means our operator will be parsed using this grammar:

```
'?' s ':' s
```


### Stack vs immediate operands
Why is `bool` pulled from the stack while `πse<π1>` is parsed? It has to do with the argument class, which is one of:

+ **Static:** things like `πi&` and `πhr_<N>`, which have nothing to do with the stack nor parsing
+ **Parse-time:** things which are not stored on the stack, e.g. `πse<π1>`, `φa...`, or `πP<T>`
+ **Runtime:** things which are stored on the stack, and which can be η-decoded, e.g. `bool`, `πhr`, and other such types (`ηauto` covers these)

Of these, only parse-time values contribute to the operator's grammar, and only runtime values contribute to the operator's stack actions. Operands are popped from the stack left-first, so `[](int, double) {...}` will pop an `int` η first, then a `double`.

Note that most runtime types can be converted to parse-time types using `πP`:

```cpp
def_sp("@", [](i64     n, πhr x) -> πhr {...});  // won't compile; this is πf<-1>
def_sp("@", [](πP<i64> n, πhr x) -> πhr {...});  // this is ok
```


### Auto-constructing a `πf<N>`
`πf<N>` takes only a `πi&` and must construct the remaining function arguments. Parse-time arguments are stored as closure data and passed in as constants since they are runtime-invariant. Statics are calculated from the `πi&`, and runtime arguments are pulled from the stack and `ηauto` converted to C++ parameters.

All of this takes the form of a nested C++ function, the outer layer of which is transformed with `φauto` (receiving `Xs&&...` arguments), the inner layer transformed with `πvauto` (receiving `Ys&&...`), and calling the user function by splicing the arguments one by one to match the original ordering. This involves `πautoclass_<T>` to classify each argument into one of the categories above and handling it accordingly.


## π function conversion
**TODO:** delete this section once it's incorporated into the above

`def_XXp` methods all define prefix operators, which are self-contained dispatch table entries that ultimately return `πf<0>` -- that is, the operand is computed first and then the operator transforms it, netting the same number of stack entries. (Plural values are always horizontal, so the stack nets zero whether singular or plural.)

Internally, `def_XXp` must insert a `φ<πf<0>>` into a dispatch table; but it's inconvenient to write parsers that return `πf` lambdas. Instead, we inline a bunch of that logic by allowing the `fn` argument to define both parse-time and runtime parameters. For example, let's create an `@n` prefix operator that selects the `n`th element from a plural operand. `n` is a parse-time integer, so our function takes a mixture of parse-time and runtime values:

```cpp
def_psp("@", [](πP<i64> n, πi &i, πp<πhr> xs) -> πhr
  { return i.i(xs, i[xs][*n]); });
```

`πP<T>` is a marker type that says "this happens at parse time"; the remaining arguments are passed to `πauto` to apply to the interpreter.

**NOTE:** these functions can also take `M<πf<1> const &>` refs, where `M<>` is a marker type, which gives them the ability to lazily evaluate their operands. I don't think we need generalized-arity πfs at this point.

`πpe<T>` is a marker type that says "this stack entry should be parsed from a plural expression". Each stack-sourced argument can have such an annotation, which is incorporated into the operator's parser.

```cpp
let f = [](πP<i64> n, πi &i, πhr xs) -> πhr
        { return i.i(xs, i[xs][*n]); };
psp_.def("@", φauto(*this, [](πP<i64> &&n)
  { return πvauto("@", [=](πi &i, πhr xs) -> πhr  // → πf<0>
    { return f(n, i, xs); }); }));
```
