# π compiler
Consumes keystrokes and produces output for the [π interpreter](pi-interpreter.md).

It's probably more accurate to think of the compiler as an IDE, but its core constraint is that it needs to resolve polymorphism before generating UTF9 sexp-style output. The assembler then converts that structure to flat bytecode.

+ [π types](pi-types.md)
+ [π assembler](pi-asm.md)


## Parse structure
The parser is a `ni`/APL hybrid: `ni` in that it uses greedy prefix matching when decoding functions and literals, APL in that functions/operators are parsed independently from their operands in most cases. Like in APL, functions have monadic and dyadic variants and are executed from left to right.
