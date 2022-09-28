# ρ
A semi-structured editor with 2.5D nonlinearity. ρ modifies UTF9 data, providing a UTF9 edit trail as you do so. This edit trail is constructed in such a way that intent can be inferred reliably, which means you can reassemble different documents easily by modifying user-edit timelines even if they were made out of sync. Multiple edit trails can be merged to a spec, for instance in such a way that a given expression is true. This may require a lot of computational effort.

+ [Editor experiments](../dev/editor-experiments.md)
+ [ρ scope](rho-scope.md)
+ L1+L2 design
  + [ASCII structure](rho-ascii.md)
  + [ρ notation](rho-notation.md)
