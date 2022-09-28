# ρ notation
As one might expect by reading the τ source code, there's going to be a lot of Unicode in π programs, some math and some simplified Chinese. Math is used for code-oriented abstractions, Chinese for human-oriented ones -- for example, you might write _∑x → 人_ to indicate that you want to show a sum of _x_ to a user. (**TODO:** this syntax may change)

The overall goal of notation is to convey the structure of a program efficiently to the person who is reading it (_读者_ or _阅读人_, if you wanted to write it as an intent). As the [L1..L4 continuum](rho-scope.md) suggests, this is done at several different levels.

π begins at the bottom, L1, so let's start with that notation. We'll build upwards from there.


## L1
π is not complicated; its low-level syntax is concatenative and mostly point-free. π is allowed to be lazy, so using maps for conditionals isn't a problem. Loops can be recursive if they don't follow some built-in pattern like `/` or `¨`.

```
fac   = ∏ι     ← ι is flexible: zero-based or one-based?
fac 5 = 120    ← sufficient to infer base case
```

API support is critical to making L1 usable. For example, `φ x` (a φ of `x`s) provides implementations for common operations like `+ 1` -- so you don't have to write any adapters to do that. Similarly, `φ a + φ b` will zip the φs and return `φ (a + b)`. Because intents and values are kind of the same thing, `ℤ + ℤ = ℤ`; therefore, `φ ℤ + φ ℤ = φ ℤ`.
