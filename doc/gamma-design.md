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

Let's use the diagram above to define the basic "operator shape", translating FDs into φ indexes:

```
                  control (φ₂)
                    ^ |
                    | |
                    | V
               +----------+
input (φ₀) <-> | operator | <-> output (φ₁)
               +----------+
                    ^ |
                    | |
                    | V
             diagnostics/debug (φ₃)
```

`input` and `output` are bidirectional; data normally moves from left to right, but sometimes the reverse happens. Most operators splice the reverse direction, since they modify only forward-moving data.

Operators can have multiple input and output channels, and can create/destroy channels at runtime (e.g. in response to control messages).


## Operator interfacing
+ `input`: externally-initiated data to be processed (like `stdin`)
+ `output`: operator-initiated results (like `stdout`)
+ `control`: externally-initiated requests to the operator to do something out of band, i.e. not steady-state (like UNIX signals and/or `stderr`)
+ `diagnostics/debug`: operator-initiated information about its operation (like `stderr`)

Because I like to use short metasyntactic names for these things, let's go with _α_, _β_, _δ_, and _ε_ for `input`, `output`, `control`, and `debug` respectively.


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

Just as with `bash`, there's a question about how we handle `δ` and `ε`. In this case we leave it up to the ⌈γ, which can route the channels through the IO ports of normal components (an important use case for bidirectional IO):

```
             δ
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
α --> |---|  f  |----|  g  |---| --> β
      |   +-----+    +-----+   |
      |                        |
      +------------------------+
```

The same principle applies to `ε`. This strategy scales because we can multiplex indefinitely, although many ⌈γs will present themselves as atomic components and won't provide introspection. That is, writing a ⌈γ doesn't necessarily entail defining a concrete API to transform its internal connections.


## Debugging and profiling
Any ζ can be multiplexed, which means we can transform any ⌈γ's internal routing to provide debugging information. We do this by multiplexing the δ and ε ports with topological tags that identify the target ⌊γ.


## Server connections
Any φ can be marked as a "server", which means that connection requests to that φ will be redirected to a different one, whose ID will be provided on the ζ connected to the server φ. It's roughly the same type of `listen`/`accept` machinery provided by Berkeley sockets, but `accept` happens automatically and no endpoint information is provided.

λs reading server φ events are not guaranteed to be scheduled in any particular order, so it isn't possible to have a server act as a reliable semaphore/mutex. (You could _ω_ all but one of the accepted connections after the fact, however.)
