# π₂
A semi-structured editor for [π₁](pi1.md) source, with 2.5D nonlinearity. π₂ modifies [UTF9](utf9.md) data, providing a UTF9 edit trail as you do so. This edit trail is constructed in such a way that intent can be inferred reliably, which means you can reassemble different documents easily by modifying user-edit timelines even if they were made out-of-sync (assuming the authors' goals were compatible). Multiple edit trails can be merged to a spec, for instance in such a way that a given expression is true. This may require a lot of computational effort, but better that than human effort.

π₂ and π₁ have overlapping designs to some degree. That's evident in the design notes, although the implementation is more clearly partitioned into π₂-as-UI and π₁-as-language.

+ [π₂ scope](pi2-scope.md)
  + [General editor experiments](../dev/editor-experiments.md)
+ L1+L2 design
  + [ASCII structure](pi2-ascii.md)
  + [π₂ notation](pi2-notation.md)
