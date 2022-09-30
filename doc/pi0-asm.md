# π₀ assembler
A stateful object that generates a preloaded bytecode interpreter. This interpreter can be stepped or run until some condition is met. The assembler allows you to incrementally construct lists and set up local variable tables.

Assemblers adapt the calling convention from C++ to π₀. For example, I might do something like this:

```cpp
π0asm a;
a.def("i64+",  [](i9 a, i9 b) { return Sc<i64>(a) + Sc<i64>(b); })
 .def("print", [](i9 a)       { cout << a << endl; })
 .q(3, 4, "i64+ print")
 .build()  // returns π0int{}
 .go();
```

`q()` takes multiple argument types and handles them in different ways:

+ Booleans, numbers, and UTF9 are quoted
+ Strings are word-split and resolved to functions
+ Initializer lists are assembled into sublists

The program above produces the list `[3 4 i64+ print]`, which is the main function and is executed when you call `.go()`.


## Local variables
π₀ is dynamically-scoped and uses local frames to provide access to variables so you don't have to rely on the stack. You create a local frame like this, which will also emit the instruction to push that frame:

```cpp
a.frame("a b c d")      // push a frame with four locals
 .q(1, "a=", 2, "b=",   // use the generated setters
    "a b i64+ c=")      // ...and getters
 .fpop()                // pop the frame when done
```

The assembler is allowed to do some trickery here: it can trace the value locations to specialize `i64+` to operate directly on locals, skipping the stack entirely. This will result in a new bytecode for `i64+` that will be used in only this instance.

Because frames are common, you can also write them inline with strings:

```cpp
a.q("[|a b c d| 1 a= 2 b= a b i64+ c=]")
```

That differs from an inline function, which doesn't introduce a frame:

```cpp
a.q("[1 2 i64+]")
```
