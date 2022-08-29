# γ
Higher-order abstraction over [φ](phi.md) and λ to create composable operators. The basic idea is to surface φs in such a way that they can be connected into standard topologies, and to bundle up λs such that we can destroy all related resources at the same time.


## Connection topology
`bash` uses a unidirectional `f | g | h` compositional pipeline, where `stderr` is untagged-unioned across each command. If we treat `stderr` as read/write (which it is), then we get this:

```
term ------+----------+----------+
           V          V          V
        +-----+    +-----+    +-----+
term -> |  f  | -> |  g  | -> |  h  | -> term
        +-----+    +-----+    +-----+
           |          |          |
           +----------+----------+-----> term
```

Let's use the diagram above to define the basic "operator shape":

```
             control
               ^ |
               | |
               | V
          +----------+
input <-> | operator | <-> output
          +----------+
               ^ |
               | |
               | V
        diagnostics/debug
```

`input` and `output` are bidirectional; data normally moves from left to right, but sometimes the reverse happens. Most operators splice the reverse direction, since they modify only forward-moving data.

Operators can have multiple input and output channels, and can create/destroy channels at runtime (e.g. in response to control messages).


## Operator interfacing
+ `input`: externally-initiated data to be processed (like `stdin`)
+ `output`: operator-initiated results (like `stdout`)
+ `control`: externally-initiated requests to the operator to do something out of band, i.e. not steady-state (like UNIX signals and/or `stderr`)
+ `diagnostics/debug`: operator-initiated information about its operation (like `stderr`)
