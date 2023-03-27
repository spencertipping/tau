# _Ξ = f(ξ)_
A Ξ is a smart cable of [ξs](xi.md) that are awaiting connections. Ξ uses a stack interface with sidecar named tables, and looks like this:

```cpp
struct Ξ
{
  Sp<Ξs> t;  // top stack entry, or null
  Sp<Ξk> m;  // next keyed Ξ, or null
  Sp<Ξk> v;  // next named variable, or null
  uN     s;  // current scope ID
};

struct Ξs
{
  ξd     io;
  Sp<Ξs> n;
};

struct Ξk
{
  η      k;
  Sp<ξd> v;
  uN     s;  // scope ID, so we can quickly pop a full scope
  Sp<Ξk> n;  // next, or null
};
```

So we focus on the stack access pattern and have variables as support. Our variable structure allows us to extend with other chains later, but we still have some coherence by focusing on the stack and allowing each meta-chain to proceed independently.

If an element needs to create new Ξs dynamically, we can leverage immutability to capture the current `m` and `v` for reference within those new Ξs. This makes it possible to add new connections to global endpoints -- see [Γ spec](Gamma.md) for details.


## Using Ξs
There are a few common operations against Ξs:

+ Primary
  + Create Ξ from input and/or output
  + Transform primary with Ψ (in either direction, or as a cap)
  + Transform primary + variable with Ψ (var as monitor/control)
  + Cap and drop primary ξ pair
  + Terminate read/write end of ξ pair
  + Bind a variable
  + Delete a variable
+ Sidecars
  + Transform sidecar with Γ
  + Multiplex keyed sidecars into a single channel
  + Combine a subset of sidecars into a multiplexed channel
  + Permute sidecar values in some way (re-key them, etc)

We also have stuff like "split Ξ into primary and tap", which is probably a Γ that binds a variable to capture the tap (or pushes it onto the anonymous stack).

I should probably outline use cases for each of these sidecar types.

+ `monitor`/`control`: `m` or `v`
+ `tap`/`join`: `s` or `v`

...what if `s` is just a stack and the primary connection is the top? Then fetching a variable is putting a new thing on top of the stack. That makes a lot of sense.

**NOTE:** Ξ scope means we don't need π scope for this, which is great. Maybe π doesn't need to be a real thing at all; we can probably just thread state through Ξ and substitute stuff with Γ operators.
