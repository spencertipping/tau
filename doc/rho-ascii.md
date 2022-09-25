# ASCII structure
Text editing hasn't changed much in the last 50 years: a document amounts to a grid of letters formed into words, lines, and sections of varying indentation. Programming languages have suffered greatly by trying to accommodate ASCII's fundamental limitations, as has documentation. So why is ASCII so enduring?

1. It's simple and easy to implement
2. It provides a broad set of editor actions that are easy to reason about
3. It maps well onto 2D screens
4. It's portable and has widespread adoption
5. It's compact on disk
6. It's easy to use with very dumb terminals, e.g. printers and typewriters
7. Every action has a simple inverse
8. When editing, there are few dead ends

Only (2), (3), and probably (7) and (8) matter for ρ. Over time ASCII has been progressively augmented into awkwardness; UTF-8, HTML/XML/Markdown, `git` and its attendant merge conflicts to manage history, bidirectional text rendering, automated refactor tools, parsers for compilers, OT for collaboration, JSON, ASCII art, and the list goes on. **This is all insane.** ASCII is the wrong tool for many jobs, yet it's so useful that we continue to use it.


## Edit topology
Let's build up a basic ergonomic ASCII editing system without any special keybindings:

1. Up/down/left/right work in visually-consistent ways
2. Shift+move = select
3. Ctrl+move = move by word
4. Type when selected = replace selection
5. Backspace/delete for backwards/forwards
6. Home/end
7. If we're getting fancy, ctrl+F to jump to content

There are only two reasons I can imagine this became so popular:

1. It's incredibly approachable for unfamiliar users
2. No piecewise/modal behavior

It's also a little underpowered, so some programmers prefer `vim` bindings. Those prioritize ergonomics (`hjkl~/`), add some programming-specific features (Ctrl+V, replace mode, region search/replace, `*`/`n`, Ctrl+{O,I}, numeric repetition, `%`, `<`/`>`), and multiple undo/redo. None of this fundamentally changes the ASCII editing model, it just constant-factor optimizes a number of operations used in technical editing.

In other words, `vim` never defies the text paradigm, with the possible minor exception of visual-block-select. It doubles down on text editing and gives the user more powerful ways to do it.

Also, a quick aside, `vim` has more dead-ends than most text editors; for example, `h` and `l` don't wrap at end of line. This seems not to be an issue. So it isn't strictly dead-ends that hamper ergonomics, but rather the probability that you'll encounter a dead end in the middle of something you're trying to do.
