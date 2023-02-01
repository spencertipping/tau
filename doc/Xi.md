# Ξ: 〈ξ〉
Mathematically, Ξ is a vector of [ξs](xi.md) used as the input and output of [γ](gamma.md) matrices.

Ξ ξs can point in either direction. This is important because some γs expect a pair of channels, one pointing each way, to perform some type of negotiation -- for example OT, throughput-dependent compression, or ED25519.


## Cabling
+ Main data connection
  + Interception with debugging/latency monitoring?
  + Circuit breakers, with their own aux inputs
  + Throughput stat collection (dedicated γ, perhaps)
+ Routing
  + Loopback (a γ)
  + Named ξs, e.g. for multiplexer branching
+ Backwards data connection
  + Unused half-duplex backlink
  + Reverse-connect a γ?
+ Auxiliary diagnostic/debug connections
  + Component-specific
  + Later routed to a multiplexer in most cases

...so we have several types of connections here:

+ Straight-through, no offshoots
  + Standard 1:1 half-duplex γ
  + Standard 1:1 full-duplex γ
+ Straight-through with a tap offshoot
  + Circuit breaker, tap = breaker control
  + Throughput stat, tap = stat output
  + Debug intercept, tap = duplicated messages
+ Tagged aux ξs
  + Bundled breakers
  + Bundled throughputs
  + Bundled debugs
  + Multiplexed versions of above

Naïvely, it seems like Ξ is defined like this:

```cpp
struct Ξ
{
  ξio f;         // →
  ξio b;         // ←
  V<ξio> fs;     // anonymous bundle of → ξs
  V<ξio> bs;     // anonymous bundle of ← ξs
  M<St, ξio> t;  // named tap ξs
  M<St, Ξ> n;    // named bundled Ξs
};
```

**NOTE:** we probably want a "full-duplex" shorthand: `ξd = P<ξio, ξio>`.

I don't think we want to split these into piecewise variations; there's no point, and there's shared context -- like, most cables will have a dedicated full-duplex data line. Any cable can accumulate a named Ξ or a separate tap line.
