# π compiler
Consumes keystrokes and produces output for the [π interpreter](pi-interpreter.md).

It's probably more accurate to think of the compiler as an IDE, but its core constraint is that it needs to resolve polymorphism before generating UTF9 sexp-style output. The assembler then converts that structure to flat bytecode.

+ [π types](pi-types.md)
+ [π assembler](pi-asm.md)


## Parse structure
The parser is a `ni`/APL hybrid: `ni` in that it uses greedy prefix matching when decoding functions and literals, APL in that functions/operators are parsed independently from their operands in most cases. Like in APL, functions have monadic and dyadic variants and are executed from right to left (i.e. are right-associative).

A type provides the parser that handles the continuation from a given point. π parsers generally aim for stability, so that types can change without breaking the parse too far.

**TODO:** does builtin version control play a part here? If we have prior versions of the same source code, we can probably use that to recover from a broken parse so we don't flood the screen with errors.
