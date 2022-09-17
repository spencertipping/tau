# π compiler
Consumes keystrokes and produces output for the [π interpreter](pi-interpreter.md).

It's probably more accurate to think of the compiler as an IDE, but its core constraint is that it needs to resolve polymorphism before generating UTF9 sexp-style output. The assembler then converts that structure to flat bytecode.

+ [π types](pi-types.md)
+ [π assembler](pi-asm.md)
