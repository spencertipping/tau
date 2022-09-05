# γ design
A discussion informing the basic principles of γ components at the API level.


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
                    control (fd 2)
                      ^ |
                      | |
                      | V
                 +----------+
input (fd 0) <-> | operator | <-> output (fd 1)
                 +----------+
                      ^ |
                      | |
                      | V
               diagnostics/debug (fd 3)
```

`input` and `output` are bidirectional; data normally moves from left to right, but sometimes the reverse happens. Most operators splice the reverse direction, since they modify only forward-moving data.

Operators can have multiple input and output channels, and can create/destroy channels at runtime (e.g. in response to control messages).


## Operator interfacing
+ `input`: externally-initiated data to be processed (like `stdin`)
+ `output`: operator-initiated results (like `stdout`)
+ `control`: externally-initiated requests to the operator to do something out of band, i.e. not steady-state (like UNIX signals and/or `stderr`)
+ `diagnostics/debug`: operator-initiated information about its operation (like `stderr`)

**TODO:** reassign greeks

Mnemonically, we can say we have four aspects: `i`, `o`, `c`, and `d` -- because we're doing Greek for abstractions, `ι`, `ο`, `ξ`, and `δ`.


## Composition
We want to be able to build larger γ structures out of smaller γ pieces. When we do, the larger structure can be written as ⌈γ and the smaller as ⌊γ.

```
    +------------------------+
    |                        |
    |   +-----+    +-----+   |
--> |---|  f  |----|  g  |---| -->
    |   +-----+    +-----+   |
    |                        |
    +------------------------+
```

Just as with `bash`, there's a question about how we handle `ξ` and `δ`. In this case we leave it up to the ⌈γ, which can route the channels through the IO ports of normal components (an important use case for bidirectional IO):

```
    +------------------------+
    |      | +-----+         |
    |      | |     |         |
    |      +-|  c  |---+     |
    |        |     |-+ |     |
    |        +-----+ | |     |
    |                | |     |
    |      +---------+ |     |
    |      |           |     |
    |      V           V     |
    |   +-----+    +-----+   |
--> |---|  f  |----|  g  |---| -->
    |   +-----+    +-----+   |
    |                        |
    +------------------------+
```

The same is true for `δ`. This strategy scales because we can multiplex indefinitely, although many ⌈γs will present themselves as atomic components and won't provide introspection. That is, writing a ⌈γ doesn't necessarily entail defining a concrete API to transform its internal connections.


## Debugging and profiling
Any ζ can be multiplexed, which means we can transform any ⌈γ's internal routing to provide debugging information. We do this by multiplexing the ξ and δ ports with topological tags that identify the target ⌊γ.
