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
true κ
```


## Ψ₂ signal dispositions
Many Ψ₂s are _linear_ in the sense that they don't interact with signals. This means any inbound signals are forwarded directly to the output. Linear processing loops can be created by using `ηlinear`:

```cpp
ξi i;
ξo o;
ηlinear(i, o, [o](ηi x)
{
  // normal logic to call o.r()...
});
```

Nonlinear loops are by their nature custom, and as a result there aren't (yet) helper functions for them.
