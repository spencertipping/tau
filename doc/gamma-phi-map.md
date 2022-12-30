# φ `*` and `|`
Many operations like JSON encode/decode are 1:1 against φ elements. Those don't require separate output φs; we can fuse their transformations into map-loops to save φ IO. It's fine with me if we restrict this optimization to strict 1:1 operations; filters can drop through φ.

Because π₀ functions are concatenative, we can φ* onto either the left or right side of the λ.
