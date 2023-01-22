# π
An interpreted, imperative bytecode language used to implement [γ](gamma.md)s.

Multiple π interpreters can share a heap, which means it's possible to send GC-managed references across [φ](phi.md).

As a concept, π covers everything from the runtime and GC up to the bytecode assembler and basic control flow. π₁ picks up from there to define the intent system and macros that compile down to this bytecode.

+ [π performance](pi-performance.md)
+ [π GC](pi-gc.md)
  + [π GC implementation](pi-gc-impl.md)
  + [π heap compression](pi-heap-compression.md)
+ [π UTF9](pi-utf9.md)
+ [π bytecode](pi-bytecode.md)
  + [π assembler](pi-asm.md)
  + [π native FFI](pi-native-ffi.md)
+ [π standard library](pi-stdlib.md)
