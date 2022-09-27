# ρ structure, part 3
Current problems:

1. Why do I have to specify edge directions manually? It should be clear from context, which linearizes the document.
2. How would I create something spatially near but semantically unrelated to some code?
3. How do I get ρ to feel like an open-ended whiteboard while preserving intentional semantics?


## Edit linearity
Point (1) above isn't so much about making edits _linear_ as it is just binding keys so I'm not wasting keystrokes typing `enter` or `space` more than I need to. So each operator should partition its continuation bindings into configuration (diagonals), auxiliaries (vertical), and composition (horizontal). In other words, build the binding set like `ni`, just render it as though directions had been specified.


## The interaction dilemma
Ignoring the problems above to talk about something different, any content-interactive editor has the problem that it mixes human canon with computer suggestion. This dilutes our human encoding: if autocomplete suggests `foo`, `bar`, and `bif` and you select `bar`, this is not the same as a human typing `bar` by hand, although maybe it's so close that we don't care.

The problem gets worse if the computer builds [intents](Pi-intent.md) from real-world observations, or does anything else that is (1) complex and (2) produces a large quoted value. The human side of that interaction is to request that some program be run against some data, which presumably is meant to be taken as a constant and frozen. So we have two consequential moments: the human specifies _how_ to get the data, then the human confirms that data as some type of compile-time artifact and commits it.

Maybe none of this is so bad, it just leads to situations where conflicting edits are difficult to resolve. And that may be a feature, not a bug. If two humans run similar processes on simliar data and get different intents, they probably need to figure out whether those differences matter and if so, which version to use.
