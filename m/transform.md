# Transform code
`ni` relies on Perl for record transformation, but I want τ to implement a custom bytecode that is better optimized for it. Ideally we get all of `ni` functionality with a more concise and targeted notation.

This is a natural fit for a stack language, so we'd use RPN and tight PEG-driven addressing. Or maybe just APL-style notation to avoid stack silliness. I think I prefer APL.


## Designing the bytecode
The first step is probably to collect `ni` commands from other places and see how we'd encode them in τ.

```sh
$ ni e[find ~ -xdev -name '*.md'] S8\<p'ru{/^\S/} if /^\$ ni/' \
     zz\>ni-commands
```
