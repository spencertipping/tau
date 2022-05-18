# Stream semantics
Durable, ordered, flow-negotiated. We can do sequence-acking if we want durability over distributed systems, but we'll need commit barriers and replay buffers. Those are extra, as is bidirectionality. At their core, streams are just atomic conveyors of [frames and records](records.md).
