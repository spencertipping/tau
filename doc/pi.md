# π
We need a native way to transform UTF9 values that doesn't result in tons of custom C++. That is, we need a little bytecode language with a simple generational GC. π source is UTF9, but with a fair bit of typesetting/layout in the frontend.


## Ideas
+ Leverage vectorization eagerly: "string → float" should produce float vectors if the string contains delimiters
+ Operators defined in terms of streams, e.g. anything that returns a tuple can stream out without collecting
+ APL-style semantics in general, where applicable
+ vi-style modality
+ Automated ergonomic profiling (doesn't have to be complicated)


### τ stream interop
+ Very easy quoting
+ Streams must be easy to refer to and connect to
+ π operates _inside_ γ, so we can use procedural access to φs


## Structure
+ Version control
  + `stream > obj` edit-fusion structure
  + Log keystrokes: that's history
  + Keystrokes → parser combinator type things, which form actions
    + Just a prefix tree, right?
  + Snapshot `obj` sometimes
  + Easy diff view: how do we do this? Union/intersection in tree space?
+ Trees with full linearity and interval selection
  + Must be able to continuously move, esp along grid coordinates
