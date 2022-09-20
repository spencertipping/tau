# π parsing
[Intent](pi-intent.md) and parsing are closely related: a successful parse is also a valid intent. Put differently, if we can describe what we expect of a result, we can also compute that result.

Parsing is allowed, and often encouraged, to be ambiguous. Underconstrained programs are then clarified, often by example. These clarifications can be separated from the program definition itself, as they're more implementation-dependent and may not capture the more mathematical core of the idea.


## Acquiring parsers
Intents provide parsers: more precisely, the initial intent, defined below, provides the UI and keybindings that map to other parsers, which are more specific. In this way, intents cause the language to vary between a shell and an algorithm definition system.

If _p(i, k) → ṗ_, then the π parser is defined as _p/_ with some initial intent. This implies a strict left → right solution, although some backtracking is allowed.


## IO
A parser is a function interrupted by a continuation: _p ∷ i →<sub>δ</sub> h/h →<sub>ε</sub> i_, where _i_ is intent, _h_ is human, and _a/b_ is a γ that consumes _a_ and produces _b_. The γ returns the next intent on ε when it's ready to commit. In this case, the _h/h_ component is the user interface; it's wired like this:

```
                 intent
                +------+
                |      |
                V      |
            +------+   |
events -+-> |  ui  | ------> rendering
        |   +------+   |
        |       |      |
        V       |      |
     history    +------+---> compiler
                 intent
```

If we want to checkpoint parse results, we can just snapshot the final intent. This avoids us having to replay the whole user input stream to reconstruct it. The π editor does this periodically to provide reasonably fast undo/redo.


### Race conditions
If _p_ runs slowly, then the X11 event queue can fill while it is generating the next intent, causing it to race the inbound events. This creates ambiguity: does the user anticipate _p_'s behavior?

Yes: events must be processed _within the intent they'd have if intents were calculated immediately._ This means the intent loop must run at least once per input event, which constrains the amount of time we can spend processing each intent, on average: about 30ms worst-case.


## _i₀_: the initial intent
_i₀_ depends on how π is being used. In most contexts you are using π from within the τ shell -- so the intent is `human` plus some information about you, if you feel inclined to provide it. If you want to pack a program for execution elsewhere, you'll need to include whatever context informed the entropy gradients you rely on; for example, "when I say `print` I tend to mean 'use `lpr`' and not 'write to console'" would be a pathological but valid preference to include in your intent.

In other words, _i₀_ should approximate your intentions for the machine(s) you're controlling as closely as is feasible; then any program you write specifies a solution from that space.

Like Ron Burgundy, _i₀_ is kind of a big deal. It's common to modify it to reflect changing infrastructural constraints, but uncommon to change your fundamental coding preferences -- at least, within the same project.


## Intents, bindings, and layers
As might be evident from the descriptions above, π is fully customizable all the way from input bindings to presentation. The intent system comprises the full bridge between user and compiler, meaning that it encapsulates everything from IDE preferences to macroexpansion and, in some cases, overload precedence.

**However,** we do not want this generality everywhere. Some bindings should never be overridden, for example `undo` or `save`. These control-bindings need to have higher priority than user preferences, even if they too are customizable. Priority implies an ordering, which is satisfied by layering.

If all of this sounds dangerous, that's because it is; but just like any π value, intents can be constrained by specific examples. _i₀_ can reject any replacements that don't implement core bindings or save things correctly.
