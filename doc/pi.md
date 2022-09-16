# π
We need a native way to transform UTF9 values that doesn't result in tons of custom C++. That is, we need a little bytecode language with a simple generational GC. π source is UTF9, but with a fair bit of typesetting/layout in the frontend.

π is an imperative language that powers a single λ, with syscall-style functions that operate various aspects of a γ -- most notably, moving UTF9 across φs. At present there's no reason for multiple π λs to interoperate; once we're doing that, we probably want to write the γ in C++.

So, succinctly: **π defines a single λ's worth of processing** and can shell out to [γ₀](gamma0.md) to create more complex topologies as necessary. This means π can easily quote [Ξ](Xi.md) outputs.

+ [π ideas](pi-ideas.md)
  + [π ← `ni`](pi-ni.md)
  + [π editor in π](pi-editor.md)
+ [π structure](pi-structure.md)
  + [π history](pi-history.md)
  + [π compiler](pi-compiler.md)
  + [π types](pi-types.md)
  + [π interpreter](pi-interpreter.md)
