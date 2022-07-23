# Trunk
A trunk is a collection of [leaves](leaf.md) within which all `pidfd` objects can move freely. Structurally, a trunk is a UNIX kernel or browser runtime; each leaf is a process or WASM worker.

Trunks _may_ elide serialization for [UTF9](utf9.md) vals, but are neither required nor expected to.

Trunks are members of a [species](species.md).
