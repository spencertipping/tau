# Λ
A cooperatively-scheduled coro manager that executes quantized units of work and guarantees fairness.


## Fair scheduling
Two things to consider:

+ _μ:_ how long does the λ tie up the system?
+ _L:_ how long has it been since its last quantum?

Broadly, I think it's fine to aim for uniformity of _L / μ_: more expensive λs can have longer latency between invocations.


## Priority adjustment
We should be able to nudge the _L_ sensitivity of any given λ by an exponential factor -- if we call that _n_, then we want _exp(n) · L / μ_ to be uniform. _n < 0_ is high-priority, _n > 0_ low-priority, just like `nice` values in Linux.
