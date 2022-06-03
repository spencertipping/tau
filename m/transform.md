# Transform code
`ni` relies on Perl for record transformation, but I want τ to implement a custom bytecode that is better optimized for it. Ideally we get all of `ni` functionality with a more concise and targeted notation.

This is a natural fit for a stack language, so we'd use RPN and tight PEG-driven addressing. Or maybe just APL-style notation to avoid stack silliness. I think I prefer APL.


## Designing the bytecode
The first step is probably to collect `ni` commands from other places and see how we'd encode them in τ.

```sh
$ ni e[find ~ -xdev -name '*.md'] S8\<p'ru{/^\S/} if /^\$ ni/' \
     zz\>ni-commands
```


## Generalizing `ni`
`ni` isn't perfect; for example, `,sgA` is a terrible construct that needs several improvements:

1. The reduction should be customizable, vs having `,sgA` and `,agA`
2. We should be able to reduce multiple columns at once, perhaps in different ways
3. We should be able to reduce the same column different ways (e.g. mean/min/max)

We really want some type of spreadsheet context to apply to groups of rows; in τ we'd create a τ group around the first column and then either do a full snapshot buffer (to run arbitrary computations) or a forward-only column reduction (to run folds).
