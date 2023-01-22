# φ `*` and `|`
Many operations like JSON encode/decode are 1:1 against φ elements. Those don't require separate output φs; we can fuse their transformations into map-loops to save φ IO. It's fine with me if we restrict this optimization to strict 1:1 operations; filters can drop through φ.

Because π₀ functions are concatenative, we can φ* onto either the left or right side of the λ: `φ*<` and `φ*>`. [γ](gamma.md) supports these inline modifications to any of its φ-connected λs. The "right" side (`φ*>`) is defined as the side closest to the φ; the "left" side points towards the inside of the γ, and the γ dictates where that ends.

**TODO:** should `φ*<` instead be equivalent to _β_'s `φ*>`, but onto _α_ instead?

`φ|` connects a φ from one γ to the φ of another γ (or of the same γ, but usually the γs are different).
