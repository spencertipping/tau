# Stream semantics
Durable, ordered, flow-negotiated. We can do sequence-acking if we want durability over distributed systems, but we'll need commit barriers and replay buffers.

Most connections are bidirectional: `A-->B` is actually `A===B`, where `B` can "reply" to `A`.
