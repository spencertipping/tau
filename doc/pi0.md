# π₀
An interpreted, imperative bytecode language used to implement [γ](gamma.md)s. Augmented by the [π₁](pi1.md) frontend for higher-level compilation.

Multiple π₀ interpreters can share a heap, which means it's possible to send GC-managed references across [φ](phi.md).

As a concept, π₀ covers everything from the runtime and GC up to the bytecode assembler and basic control flow. π₁ picks up from there to define the intent system and macros that compile down to this bytecode.

+ [π₀ performance](pi0-performance.md)
+ [π₀ GC](pi0-gc.md)
  + [π₀ GC implementation](pi0-gc-impl.md)
+ [π₀ UTF9](pi0-utf9.md)
+ [π₀ bytecode](pi0-bytecode.md)
  + [π₀ assembler](pi0-asm.md)
  + [π₀ native FFI](pi0-native-ffi.md)
+ [π₀ standard library](pi0-stdlib.md)
