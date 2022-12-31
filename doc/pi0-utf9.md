# π₀ UTF9
[UTF9](utf9.md) reserves the `11000___` prefix for π use. These values are scoped down to the π₀ context; that means we can use native endianness and sizing. The only constraint is that we be able to remove any such values from UTF9s we're working with, e.g. before sending them across a [φ](phi.md).

Within a π execution context, π-scoped UTF9s are the only values that are allowed to set the UTF9 flag.

Like other scoped values, π₀ UTF9s begin with a byte that defines their purpose.

+ `00`: reserved
+ `01`: π₀ heap reference (`π0r`)
+ **TODO:** `02`: π₀ mutable heap reference (`π0r`)
+ `10`: π₀ bytecode offset (`π0bi`)


## References
`01` references are _soft_ in that they are allowed to be rewritten into their referents. This makes them immutable; you can't change the referent after they are created.

`02` references require explicit dereferencing and are not implicitly rewritten. They're much more like C pointers, but they refer to on-heap values and participate in GC.
