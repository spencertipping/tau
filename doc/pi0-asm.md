# π₀ assembler
A stateful object that generates a preloaded bytecode interpreter. This interpreter can be stepped or run until some condition is met. The assembler allows you to incrementally construct lists and set up local variable tables.

Assemblers adapt the calling convention from C++ to π₀. For example, I might do something like this:

```cpp
π0asm a;
a.def("i64+",  [](i9 a, i9 b) { return Sc<i64>(a) + Sc<i64>(b); })
 .def("print", [](i9 a)       { cout << a << endl; })
 .q(3, 4, "i64+", "print")
 .build()  // returns π0int{}
 .go();
```

`q()` takes multiple argument types and handles them in different ways:

+ Booleans, numbers, and UTF9 are quoted
+ Strings are resolved to functions

The program above produces the list `[3 4 i64+ print]`, which is the main function and is executed when you call `.go()`.


## Local variables
π₀ is dynamically-scoped and uses local frames to provide access to variables so you don't have to rely on the stack. You create a local frame like this, which will also emit the instruction to push that frame:

```cpp
a.frame("a", "b", "c", "d")  // push a frame with four locals
 .q(1, "a=", 2, "b=",        // use the generated setters
    "a", "b", "i64+", "c=")  // ...and getters
 .fpop()                     // pop the frame when done
```

The assembler is allowed to do some trickery here: it can trace the value locations to specialize `i64+` to operate directly on locals, skipping the stack entirely. This will result in a new bytecode for `i64+` that will be used in only this instance.


## Inline syntax
C++ delimiters are clunky, so we have a small syntax that can be used to bypass C++ and write programs inline.

```cpp
a << "1 2 i64+ print"  // .q(1, 2, "i64+", "print")
```

You can also write frames and lists:

```cpp
a << "3 4 5 [|a b c| a b i64+] ."
  // .q(3, 4, 5)
  // .begin()
  //   .frame("a", "b", "c")
  //   .q("a", "b", "i64+")
  //   .fpop()
  // .end()
  // .q(".")
```

Because the syntax is parsed incrementally, `[` and `]` need not be matched within any given string. The assembler will fail, however, if you `.build()` with unclosed brackets or frames.
