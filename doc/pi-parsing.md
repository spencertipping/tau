# π parsing
[Intent](pi-intent.md) and parsing are closely related: a successful parse is also a valid intent. Put differently, if we can describe what we expect of a result, we can also compute that result.

Parsing is allowed, and often encouraged, to be ambiguous. Underconstrained programs are then clarified, often by example. These clarifications can be separated from the program definition itself, as they're more implementation-dependent and may not capture the more mathematical core of the idea.


## Acquiring parsers
Intents provide parsers: more precisely, the initial intent (which I think is some combination of `human + version controlled π source` -- **TODO**) provides the UI and keybindings that map to other parsers, which are more specific. In this way, intents cause the language to vary between a shell and an algorithm definition system.

If _p(i, k) → ṗ_, then the π parser is defined as _p/_ with some initial intent.


## IO
A parser is a function interrupted by a continuation: _p ∷ i → h/h →<sub>ε</sub> i_, where _i_ is intent, _h_ is human, and _a/b_ is a γ that consumes _a_ and produces _b_. The γ returns the next intent on ε when it's ready to commit.

If we want to checkpoint parse results, we can just snapshot the final intent. This avoids us having to replay the whole user input stream to reconstruct it. The π editor does this periodically to provide reasonably fast undo/redo.
