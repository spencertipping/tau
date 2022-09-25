# ρ structure, part 2
Picking up from the discussion and implied constraints in [part 1](rho-structure1.md), let's now try to figure out how ρ documents should work. It's a complicated and open-ended space, so this should be a lot of fun.

Iverson has some interesting presentation in the APL book: although he sometimes lists code in a conventional way, it's often embellished with control-flow arrows and ends up looking more like a graph than like a text document. This is semantically correct: control flow is poorly represented by text and better by arrows. `if`-body indentation and other layout is a symptom of this; we want visual presentation to reflect semantic structure.

I don't believe ρ needs to have an opinion about how semantics should be represented (π and ρ are split in part 1), but I do think it should provide ways to view code that align with semantic meaning.


## Input gestures
+ Mouse inputs (imprecise, but good at angle+magnitude)
+ Scroll inputs (imprecise, adds third and fourth dimensions to mouse)
+ Touch inputs (imprecise, but good at multi-selection)
+ Keyboard inputs (precise but awkward for spatial indication)

I'm going to stick to keyboard inputs for ρ. We can have mouse/touch too, but the editor should, like `vim`, be completely usable without those. Mouse/keyboard switching overhead is high enough that we should allow the user to focus on the keyboard.


## Angular structure
Math uses a roughly octagonal alignment: most adjacency is horizontal, some is vertical, and exponents and subscripts are right-diagonal. Only occasionally are left-diagonals used, e.g. for integral evaluation bars. LaTeX defines fairly standard keyboard shortcuts for this arrangement: `^` for superscripts, `_` for subscripts, and if we have up/down then we might use `^^` and `__`. Left-diagonals might be `^<` and `_<`, not sure. I'm also not sure we'll use these.

ASCII prefers horizontal to vertical linkage -- that is, horizontally-adjacent words relate to each other more than vertically-adjacent lines do. That matters for range selection: if I select text and shift-select upwards, I'm assumed to mean "select until you get up", not "select the column vertically". This intention becomes less clear if text has octagonal continuations; it's definitely nonlinear, so the linear-region selection heuristic isn't meaningful.

There's also a question about what it even means to select vertically vs horizontally-related content. For example, if I move `a(^^b) c`, am I left with `c` or `a c`? Probably `a c` because `a` still has outbound edges -- but maybe not if `a` is also selected. And if we select `c`, are we selecting `^^c` or just `c` itself?

One strength of ASCII is that from an editing perspective it's strongly concatenative; cut/paste is always well-defined. Nonlinear or multidirectional selections break that property. Of course, a terse language is less likely to require fragmented copy/paste; generally we'll be moving whole forms. So we can have term-affinity and later constraint-driven DWIM. Or maybe we don't have copy/paste as such; going back to the atomicity discussion from part 1, maybe we open a wormhole between locations and move or duplicate stuff across a boundary. So copy/paste can be seen as "collapse space and move something a short distance" rather than "grab stuff and drop it into an arbitrary other location." Or drop content into a 2.5D layer and shift that layer around -- then we have content tracking.


## Nonlinear continuations
ASCII's nonlinear (vertical) selection is often not useful, but it feels responsive in that it gets close to your cursor position even when it's a gross, imprecise movement. It never feels like you enter a null space -- not until you run off the right margin, at which point it can't keep up with you. The ConTEXT editor allowed you to click off the end of a line; it wasn't very useful most of the time, but it made the editor feel more dynamic and was kinda cool.

If we're selecting with the mouse, the selection must converge to the point. And if we're selecting with the keyboard, we should probably use some type of geometry that back-applies to the document. This allows selection to exploit folding within the view, which is critical: if you cared about folded contents you'd unfold before selecting.


## Coordinates
ASCII uses numeric coordinates, which produces an awkward OT. ρ will do better by anchoring each element relative to something else. Those relative edges are OT-strong, so each element has a single anchoring path back to the origin. I don't think I want to support multiple anchoring paths; although they might be useful as indicators, we should have the π language catch this instead of ρ.

Coordinates are therefore paths back to "ground". They aren't static; each character is linked to exactly one other character, and you can insert more characters to relink edges. Graphically:

```
a ← b ← c           < arrows point to ground
a ← b ← d ← c       < insert d before c
a ← d ← c           < delete b
```


## Symbols
This is more about mathematical notation than it is about anything else, but each "cell" can have more than one character in it because we have a rich character set. So if you type `abc def`, you'll get two cells: `abc ← def`. And if you type `abc^def`, you'd get this:

```
   def
abc
```

Some letter combinations turn into symbols; most `vim` digraphs can be written verbatim. So if you type `s* *l`, you'll get `σ ← λ`.


## Edge direction
Edge direction matters, so we need a way to communicate it. We do this with two mechanisms: first, anything you select will highlight its immediate anchor; and second, every inbound edge increases the font size of its target (non-transitively).

Mappings for directions:

| Key         | Direction  |
|-------------|------------|
| space       | right      |
| tab         | left       |
| shift+enter | up         |
| enter       | down       |
| ^           | up+right   |
| _           | down+right |
| ^^          | up+left    |
| __          | down+left  |

`^` and `_` alternate between left and right if further repeated.

Some keybindings change modes; `"` begins a string that continues until you type the closing `"`. This allows you to type `^` and `_` without breaking the cell.

`hjkl` work to move around, with `HJKL` traversing diagonals. `HJKL` behave as though they were rotated 45° leftwards (counterclockwise) from the `hjkl` basis.

**TODO:** write this editor prototype
