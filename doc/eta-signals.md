# η signal conventions
Before I get into the specifics how signals are handled, let's outline what they are for in the first place.

Signals are regular data that typically appear by themselves if they are intended to apply to an active data stream. For example, suppose you read these values from a ξ:

```
1
2
3
τ
4
5
τ
```

We have two τ-groups of numbers: `1 2 3` and `4 5`. The semantics of τ depend on context, but it always refers to some sort of state reset. The metaphor is that the stream has completed a cycle, hence the mnemonic τ (the circle constant), meaning "one turn".

τ markers are useful in situations where a stream carries locally-ordered data but an indefinite lifetime. τ indicates that the ordering has been reset, or that a new group is now being processed.


## Signals inside other data
You can embed a signal inside other data, at which point it has no default effect on the stream carrying it. For example, these messages are all treated as regular data:

```
1 2 τ
"foo" ω
true κ 2 3 τ 5
```

The same is true if you have a sub-η: `(τ)` is also regular data.

**NOTE:** signals apply if they are the first element in the η stream, even if there are other elements after them. So `τ 1 2 3` is a τ signal against its stream.


## Ψ₂ signal dispositions
Many Ψ₂s are _linear_ in the sense that they don't interact with signals. This means any inbound signals are forwarded directly to the output. Linear processing loops can be created by using `ηl`:

```cpp
ξi i;
ξo o;
ηl(i, o, [&](ηi x) { o.r() << x.i() + 1; });
```

Another common pattern is to have some effect that occurs on τ cycles. For example, suppose you want to sum each τ-delimited group and emit just the totals. That logic could be written like this:

```cpp
ξi i;
ξo o;
i64 t = 0;
ητ(i, o,
   [&](ηi x) { t += x.i(); },          // handle normal data
   [&]()     { o.r() << t; t = 0; });  // handle τ signal
```
