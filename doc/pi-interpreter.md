# π interpreter
Consumes the output of the [π compiler](pi-compiler.md), which is a C-style imperative structure that we SSA down to garbage-collected register bytecode. I would use LLVM, but it doesn't support JIT in WebAssembly to my knowledge.

π interpreter values are all heap-allocated UTF9s that never refer to one another. This means GC is a flat loop over the register table, making generational GC trivial to implement. It also means that a π interpreters are data: they can be suspended to disk, moved to different machines, forked, etc.


## Interpreter input format
A UTF9 tree structure that refers to concrete functions by index, but allows subexpression trees. There are a few special forms that are translated by the assembler when constructing bytecodes:

+ `(do ...)`: sequential execution
+ `(label x (...))`: define a label `x` (a symbol) that can be jumped to
+ `(offsetof x)`: refer to the bytecode offset of a label
+ `(global x)`: translate `x` (a symbol) into a global register index

Everything else is a function call that refers to one of the native functions in the interpreter's lookup table.


## Interpreter structure
π programs have four elements:

1. The program metadata and bytecode (see below for format)
2. Global registers, which can be modified randomly
3. The data register stack, which can be modified using `push`/`pop`
4. The return stack, which can be modified using `push`/`pop`

A π program must declare the number of global registers it needs before executing.

The top of the return stack always refers to the next instruction to be executed.

"Data registers" and globals are both global, but data registers are optimized to be used for temporary expressions: when popped, they rewind the heap. This means that, in many cases, values stored in data registers are effectively stack-allocated. (The main exception being when you update global registers in the middle, which pins the rewind-point to that value. I may use separate heaps for globals vs data registers to avoid this.)

If I add multi-π shared global support in the future, global registers will be shared between π programs and the data/return stacks will be π-local.


### Metadata format
Not much, just a small UTF9 structure that defines a few things:

1. The total number of global registers
2. The offset of the first instruction to execute (start address)


### Bytecode format
Bytecode is simple: an immutable list of `u32`s in native-endian encoding, packed as a flat array. Instructions are therefore always four-byte aligned. Each instruction is an offset into the π native table, and has a fixed size. This means bytecode can easily be scanned without executing it.

To satisfy the fixed-arity requirement, forms like `(tuple, ...)` end up compiling to bytecodes specialized to the number of elements being stored -- e.g. `tuple4 1 2 3 4`. Dynamically-sized or large tuples can be created by instructions like `tuple-append`.
