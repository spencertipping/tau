# π design
As mentioned in [π](pi.md), we need a zero-copy frame of reference so we can transform individual UTF9 values without incurring piecewise overhead by copying stuff around. Imperative π should be in the same performance ballpark as Python/Perl/Ruby.

π exists to provide convenient and reasonably fast UTF9-transformation operators, which means it should be imperative and garbage-collected. The [π compiler](pi-compiler.md) should do some work to reduce complex forms down to simple ones -- i.e. most of the optimization effort belongs in the compiler rather than in the [bytecode interpreter](pi-interpreter.md).

[γ](gamma.md) operates in an implicitly vectorized context because values arrive via [φ](phi.md). The compiler should choose vectorizable bytecodes when possible to minimize interpreter overhead.

+ [Reddit analysis ETL](pi/reddit.md)
