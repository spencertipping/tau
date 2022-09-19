# π parsing
[Intent](pi-intent.md) and parsing are closely related: a successful parse is also a valid intent. Put differently, if we can describe what we expect of a result, we can also compute that result.

Parsing is allowed, and often encouraged, to be ambiguous. Underconstrained programs are then clarified, often by example. These clarifications can be separated from the program definition itself, as they're more implementation-dependent and may not capture the more mathematical core of the idea.


## Acquiring parsers
Intents provide parsers: more precisely, the initial intent (which I think is some combination of `human + version controlled π source` -- **TODO**) provides the UI and keybindings that map to other parsers, which are more specific. In this way, intents cause the language to vary between a shell and an algorithm definition system.

So parsers are bidirectional φs that consume parse input -- i.e. events -- and produce UI requests to render their state.
