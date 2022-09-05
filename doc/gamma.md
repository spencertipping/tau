# γ
Higher-order abstraction over [φ](phi.md) and λ to create composable operators. The basic idea is to surface φs in such a way that they can be connected into standard topologies, and to bundle up λs such that we can destroy all related resources at the same time.

The boot γ within each [Φ](Phi.md), γ₀, accepts commands to construct γ components and tie them together.


## Design elements
### Connection topology
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


### Operator interfacing
+ `input`: externally-initiated data to be processed (like `stdin`)
+ `output`: operator-initiated results (like `stdout`)
+ `control`: externally-initiated requests to the operator to do something out of band, i.e. not steady-state (like UNIX signals and/or `stderr`)
+ `diagnostics/debug`: operator-initiated information about its operation (like `stderr`)

Mnemonically, we can say we have four aspects: `i`, `o`, `c`, and `d` -- because we're doing Greek, `ι`, `ο`, `ξ`, and `δ`.


### Composition
We want to be able to build larger Γ structures out of smaller γ pieces:

```
    +------------------------+
    |                        |
    |   +-----+    +-----+   |
--> |---|  f  |----|  g  |---| -->
    |   +-----+    +-----+   |
    |                        |
    +------------------------+
```

Just as with `bash`, there's a question about how we handle `ξ` and `δ`. In this case we leave it up to the Γ, which can route the channels through the IO ports of normal components (an important use case for bidirectional IO):

```
    +------------------------+
    |      |                 |
    |      |  +----+         |
    |      +--| c  |---+     |
    |         |    |-+ |     |
    |         +----+ | |     |
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

The same is true for `δ`. This strategy scales because we can multiplex indefinitely, although many Γs will present themselves as atomic components and won't provide introspection.


### Debugging and profiling
Any ζ can be multiplexed, which means we can transform any Γ's internal routing to provide debugging information. We do this by multiplexing the ξ and δ ports with topological tags that identify the target γ.


## Authoring γ components
Right now I write γ components like this:

```cpp
struct iota
{
  γ  g;
  uN n;
  iota(Φ &f, uN n_ = -1) : g(f), n(n_)
    { g.λc([&]() {
        for (uN i = 0; i < n; ++i)
          g.ο() << i;
        g.ο().ω();  // very important (though it shouldn't be)
        return 0;
      }); }
};

struct delay
{
  γ  g;
  ΔΘ dt;
  delay(Φ &f, ΔΘ dt_) : g(f), dt(dt_)
    { g.λc([&]() {
        for (let x : g.ι()) g.Θ(dt).ο() << x;
        g.ο().ω();
        return 0;
      }); }
};
```

Each operator has very little going on; it should be possible to do something like this instead:

```cpp
γ &iota(uN n)
{ return Γ([n](γ &g, γφ, γφ o) {
    for (uN i = 0; i < n; ++i) o << i;
    // all ports auto-closed when this λ returns
  }); }

γ &delay(ΔΘ dt)
{ return Γ([dt](γ &g, γφ i, γφ o) {
    for (let x : i) g.Θ(dt), o << x;
  }); }
```

It's probably worth some iteration to figure this out.


### Structure
+ One or more λs
+ Init arg, copied in by value (toplevel k/v structure)
+ Runtime storage shared across λs

Also worth considering: we may want some type of unified structure to initialize, rather than having open-ended C++. For example, we might insist on `i9`. We also want to store these things in a dispatch table rather than defining C++ symbols directly.

```cpp
defγ(iota) {
  uN n = Ξ("n", -1);
  for (uN i = 0; i < n; ++i) o << i;
};
```

The main λ can create others with `g.λc()`, and it can manage memory and create any local heaps that might be needed. `Ξ` accesses individual config options. If the main λ exits, then the γ is automatically destroyed (which closes its φs).

γ also converts the `return n` interface to a more UNIX-like exit-from-anywhere setup. Any of the participating λs can invoke `g.x()` to implode the γ, cleaning up all resources and (**TBD**) setting the final state of the γ, which can be observed by whoever created it.


### γ interfacing
1. How are γs created and later identified?
2. How does a γ exit normally vs abnormally, if at all?
3. If we have compound/nested γs, who manages the compound linkages?
4. For γs that have multiple φs, how are those φs declared before γ₀ creates the ζ linkages?


### γ₀
First, each τ program has a bootup γ, γ₀, that accepts further UTF9-encoded instructions to create γs and connect them. Each connection request consists of two pieces:

1. A map of `γid → (name config-map)` defining the γs we intend to create
2. A list of `(γid1, φ1, γid2, φ2)` tuples defining the space of ζs

Addresses are unique to a specific connection request. The γ₀ will reply with another mapping of `γid → (Γid = (Φid, Φγid))` that anchors each request-local γ ID to a permanent and globally unique one.
