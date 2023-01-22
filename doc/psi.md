# ψ resource scope
ψ contains a few different types of objects:

+ λ IDs
+ (Symbol → heap reference) global table
+ `shared_ptr` to various C++ natives:
  + `V<η*>`
  + `S<η*>`
  + `M<η*, η*>`
  + `B`
  + `regex`, probably

C++ natives can be moved/referenced anywhere within the same π. Although the C++ objects are τ-portable, all except `B` and `regex` hold references to values on the π heap, so they would become invalid outside that context.


## Lifecycle
ψs can collect resources as long as they are live; when they are destroyed, their resources are deallocated in this order:

1. λs are destroyed; no further code can run
2. All `shared_ptr`s are destroyed, decrementing their refcounts
3. Global table is destroyed

After this, the ψ object itself is destroyed and removed from its containing [π](pi.md), which may cause the π to be destroyed.
