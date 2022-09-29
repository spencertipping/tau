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

**NOTE:** this equivalence conflicts with common mathematical notation like _ℝ²_ or _ℝ × ℝ_: multiplying two values differs from multiplying two sets. I think this is fine, but it's probably worth seeing if there's a way to fix the discrepancy.


### Layout
Text flow should follow natural typographic layouts: Latin scripts proceed left to right, Chinese top to bottom (which can be coerced LTR when necessary).

Math is often typeset multidirectionally, and we try to reflect that. Although L1 input is linear, we offset superscripts and subscripts, e.g. `β²`. Same for certain vertical digraphs like `=` with some modifier above or below.

It's possible I'll want to do more advanced chaining of larger symbols like `∑`, but I'm not so sure. I think the typesetting logic can be simple enough: vertical/horizontal alignment and "append to bounding box". The `i ∈ 1:10` type of qualifier you often see on ∑ would be a child box in layout, although its input would just be `_i<1:10` or similar.

A note about ergonomics: flex-layout is common, and I don't want to have awkward keybindings like `vim`'s Ctrl+K digraph prefix. Instead, we should either have no prefix at all (e.g. for Chinese, which can be Pinyin word-detected), or something simple like `\`.


### Framing
Layout applies within a text-flow context, but those contexts themselves can be placed into a larger arrangement using frames. Frames assign positions to things they contain, some of which may be other frames.

Frames don't have intrinsic boundaries or even a shape. They just assign an origin and use some strategy to assign coordinates to their contents. Therefore, a frame can be moved and otherwise transformed.

**FIXME:** "frame" is the wrong abstraction; it's too concrete
