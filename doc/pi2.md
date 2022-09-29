# π₂
A semi-structured editor for [π₁](pi1.md) source, with 2.5D nonlinearity. π₂ modifies [UTF9](utf9.md) data, providing a UTF9 edit trail as you do so. This edit trail is constructed in such a way that intent can be inferred reliably, which means you can reassemble different documents easily by modifying user-edit timelines even if they were made out of sync. Multiple edit trails can be merged to a spec, for instance in such a way that a given expression is true. This may require a lot of computational effort.

+ [General editor experiments](../dev/editor-experiments.md)
+ [π₂ scope](pi2-scope.md)
+ L1+L2 design
  + [ASCII structure](pi2-ascii.md)
  + [π₂ notation](pi2-notation.md)
