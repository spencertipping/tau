# Ξ: 〈ξ〉
Mathematically, Ξ is a vector of [ξs](xi.md) used as the input and output of [γ](gamma.md) matrices.

Ξ ξs can point in either direction. This is important because some γs expect a pair of channels, one pointing each way, to perform some type of negotiation -- for example OT, throughput-dependent compression, or ED25519.


## Half and full duplex
ξs are always carried in duplexed pairs, and a Ξ contains several elements:

1. The primary data pair
2. A series of anonymous pairs, e.g. to collect circuit breakers
3. A series of named pairs, e.g. to collect specific multiplexer
4. A series of named Ξs
