# π₀ assembler
A stateful object that generates a preloaded bytecode interpreter. This interpreter can be stepped or run until some condition is met. The assembler allows you to incrementally construct lists and set up local variable tables.

Assemblers adapt the calling convention from C++ to π₀. For example, I might do something like this:

```cpp
π0asm a;
a.def("i64+",  [](i9 a, i9 b) { return Sc<i64>(a) + Sc<i64>(b); })
 .def("print", [](i9 a)       { cout << a << endl; })
 .lit(3, 4)
 .fn("i64+", "print")
 .compile()  // returns π0int{}
 .go();
```

`lit` is a builtin that quotes a UTF9 value onto the stack -- or more specifically, generates a function that does this. Any value you pass it is copied and held as static data within the resulting interpreter.

You can assemble sublists, e.g. for conditionals: those are done depth-first using `.list()` and `.end()`.


## Local variables
π₀ is dynamically-scoped and uses local frames to provide access to variables so you don't have to rely on the stack. You create a local frame like this:

```cpp
a.frame("a b c d");  // create a frame with four locals
```
