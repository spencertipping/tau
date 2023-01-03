# π₁ code
**π₁ code does not have native control flow.** It borrows from TCL and Joy by having blocks be quoted concatenative functions that are applied using dynamic scope. That is, there are instructions that add and remove local-variable stack frames, and "names" are aliased to local indexes by the assembler. It's a very simple world. See [π₀ bytecode](pi0-bytecode.md) for details.
