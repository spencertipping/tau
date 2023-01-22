# φ: 〈μ/ξ〉
Mathematically, φ is a vector of [channels](muxi.md) used as the input and output of [γ](gamma.md) matrices.

Structurally, φ is a sometimes-recursive symbol-keyed map of channel proxy objects. These track the direction and opposite endpoint of each channel so we can create μ when possible; otherwise we'll create ξ.

φ channels can point in either direction. This is important because some γs expect a pair of channels, one pointing each way, to perform some type of negotiation -- for example OT, throughput-dependent compression, or ED25519.
