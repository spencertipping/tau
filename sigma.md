# σ: the τ standard library
σ defines common utilities that make τ ergonomic and useful. This includes [Γ](doc/Gamma.md) components and [φ](doc/phi.md) parsers. The `σ` namespace doesn't include `τ` by default. Most programs don't need to import `τ`, as `σ` provides entry points for common operations.


## TODO
+ HTTP library
  + HTTP/REST _requests_ are a stream, which can be sub-streamed for WS
  + Authentication is a fork
+ π scripting
  + ← for Γ assignment, refer to them later
+ Generalize Ξ topology in Γ layer
  + Any `a → b` should be forward, backward, left-loop, or right-loopable
  + Do this by having Γ2 with different (Ξ → P<ξi, ξo>) functions
  + We can generalize further by having other Γ numbers
