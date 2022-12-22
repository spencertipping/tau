# λ/φ idioms
[π₀](pi0.md) is all about creating components that can be reused and connected easily, with a well-defined dataflow topology. `ni` does this in a limited way, but τ has more complexity:

+ Topology sections can be created and destroyed at runtime
+ Connections aren't just to input/output
+ Components can claim resources and/or use persistent resources

These idioms are primarily managed by [γ](gamma.md).
