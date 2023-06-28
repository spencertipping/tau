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


## Transformers
**TODO:** delete transformers as an idea. There's no point; we can just auto-fill them to `x...`, like Perl does with its `$_` variable.

In addition to normal expressions, which increase the stack depth by one, π also provides _transformers_, which are net-zero code snippets that assume an implied operand. Transformers make it possible to write `+1` to mean "a function which adds one to something".

**TODO:** define transformers' behavior for plural values, if that's a thing

**TODO:** define π stream transform behavior: we could use transformers?

**TODO:** should this idea even exist? Seems low-overhead enough to just bind a variable

How about `+1` turns into `x+1`, and we use a persistent "it register" `x`, like in Perl?


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
def_sp("?", [](πi &i, πse<π1> const &t, φaL<':'>, πse<π1> const &e, bool c)
  { c ? t.x(i) : e.x(i);
    return i.pop(); });
```

There's a lot going on here. First, each parameter may introduce a parser into the operator sequence. In this case we have three: `πse<π1>`, `φaL<":">`, and `πse<π1>`. `bool` is assumed to be present on the stack already since it's the left operand. This means our operator will be parsed using this grammar:

```
'?' s ':' s
```


### Stack vs immediate operands
Why is `bool` pulled from the stack while `πse<π1>` is parsed? It has to do with the argument class, which is one of:

+ **Meta:** things like `πi&` and `πhr_<N>`, which have nothing to do with the stack nor parsing
+ **Constant:** things which are not stored on the stack, e.g. `πse<π1>`, `φa...`, or `πP<T>`
+ **Immediate:** things which have an expression-mode annotation for parsing and are stored on the stack, e.g. `πse<i64>`
+ **Stack:** things which are stored on the stack up front, and which can be η-decoded, e.g. `bool`, `πhr`, and other such types (`ηauto` covers these)

Each immediate is inlined as a `πf<1>` that is prepended to the resulting function. This results in immediates being present above stack arguments. As a result, arguments must be ordered like this:

```
meta* (const | immed)* stack*
```

Immediate and stack arguments are unpacked top-to-bottom. This means immediates are prepended right-to-left, then all arguments are unpacked in interpreter stack order (with interspersed constants as appropriate).


### Auto-constructing a `πf<N>`
`πf<N>` takes only a `πi&` and must construct the remaining function arguments. Parse-time arguments are stored as closure data and passed in as constants since they are runtime-invariant. Statics are calculated from the `πi&`, and runtime arguments are pulled from the stack and `ηauto` converted to C++ parameters.

All of this takes the form of a nested C++ function, the outer layer of which is transformed with `φauto` (receiving `Xs&&...` arguments), the inner layer transformed with `πvauto` (receiving `Ys&&...`), and calling the user function by splicing the arguments one by one to match the original ordering. This involves `πautoclass_<T>` to classify each argument into one of the categories above and handling it accordingly. See [pi-auto](../tau/pi-auto.hh) for implementation.
