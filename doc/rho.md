# ρ
A semi-structured editor with 2.5D nonlinearity. ρ modifies UTF9 data, providing a UTF9 edit trail as you do so. This edit trail is constructed in such a way that intent can be inferred reliably, which means you can reassemble different documents easily by modifying user-edit timelines even if they were made out of sync.


## ASCII structure
Text editing hasn't changed much in the last 50 years: a document amounts to a grid of letters formed into words, lines, and sections of varying indentation. Programming languages have suffered greatly by trying to accommodate ASCII's fundamental limitations, as has documentation. So why is ASCII so enduring?

1. It's simple and easy to implement
2. It provides a broad set of editor actions that are easy to reason about
3. It maps well onto 2D screens
4. It's portable and has widespread adoption
5. It's compact on disk
6. It's easy to use with very dumb terminals, e.g. printers and typewriters
7. Every action has a simple inverse

Only (2), (3), and probably (7) matter for ρ. Over time ASCII has been progressively augmented into awkwardness; UTF-8, HTML/XML/Markdown, `git` and its attendant merge conflicts to manage history, bidirectional text rendering, automated refactor tools, parsers for compilers, OT for collaboration, JSON, ASCII art, and the list goes on. **This is all insane.** ASCII is the wrong tool for many jobs, yet it's so useful that we continue to use it.


## ρ structure
ρ documents are built by replaying edits, each of which is a UTF9 value in a streaming timeline. Edits correspond to individual X11/input events; they are considered to be completely atomic from the user's perspective. Documents are allowed to incorporate author/time information and other metadata into the assembled structure. It's UTF9, after all, and UTF9 is flexible.

Because a document is a UTF9 data structure, it can also be snapshotted to avoid replay overhead. In this way UTF9 documents behave like git repositories.

Documents also need not be linear. One "document" may be many logical trees or regions of content, possibly with links between them. Each piece can be branched and reassembled arbitrarily; edit streams are first-class objects that can be transformed with [π](pi.md) functions.


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

1. It's incredibly simple
2. No piecewise/modal behavior

It's also a little underpowered, so some programmers prefer `vim` bindings. Those prioritize ergonomics (`hjkl~/`), add some programming-specific features (Ctrl+V, replace mode, region search/replace, `*`/`n`, Ctrl+{O,I}, numeric repetition, `%`, `<`/`>`), and multiple undo/redo. None of this fundamentally changes the ASCII editing model, it just constant-factor optimizes a number of operations used in technical editing.

In other words, `vim` never defies the text paradigm, with the possible minor exception of visual-block-select. It doubles down on text editing and gives the user more powerful ways to do it.
