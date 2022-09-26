# ρ structure, part 3
Current problems:

1. Why do I have to specify edge directions manually? It should be clear from context, which linearizes the document.
2. How would I create something spatially near but semantically unrelated to some code?
3. How do I get ρ to feel like an open-ended whiteboard while preserving intentional semantics?


## Edit linearity
Point (1) above isn't so much about making edits _linear_ as it is just binding keys so I'm not wasting keystrokes typing `enter` or `space` more than I need to. So each operator should partition its continuation bindings into configuration (diagonals), auxiliaries (vertical), and composition (horizontal). In other words, build the binding set like `ni`, just render it as though directions had been specified.
