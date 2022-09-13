# π structure
+ Version control built into the language
  + `stream > obj` edit-fusion structure
  + Log keystrokes: that's history
  + Keystrokes → parser combinator type things, which form actions
    + Just a prefix tree, right?
  + Snapshot `obj` sometimes
  + Easy diff view: how do we do this? Union/intersection in tree space?
  + Reconstruct history → `reflog` with full history
+ Trees with full linearity and interval selection
  + Must be able to continuously move, esp along grid coordinates: that is, a single Cartesian direction should rarely have a stop point


## Mechanics
+ Trivial `eval` function
  + Evaluation is sequential with stack-like context
  + Register-style variables
  + `eval` instead of functions, like Joy
+ Declarative ξ topology
+ APL-style declarative operator mods: `/+` to sum, for example
  + Prefix-tree parsed: `/` accepts `<binop>` as a successor, give or take
  + Polymorphic interpretation: `/+1 2 3` sums a vector, `/+α` sums elements in the _α_ stream
  + Static typing with easy coercion
+ Generational, stop-the-world, mark/sweep GC

**Q:** is UTF9 fast enough to use as the bytecode format, or should we have a dedicated π bytecode? UTF9 is nice and homoiconic, so I'd like to use it if we can. Maybe trace → JIT the decoding process if it's too slow.
