# Stream semantics
Durable, ordered, flow-negotiated. We can do sequence-acking if we want durability over distributed systems, but we'll need commit barriers and replay buffers. Those are extra, as is bidirectionality. At their core, streams are just atomic conveyors of [frames and records](records.md).

Each stream can specify its "maximum atomic write" capacity, which must be at least 512 bytes. This makes it compatible with even the lowest values of `PIPE_BUF` on UNIX systems. Typically, a stream that offers very low atomic-write capacity will be wrapped with a byte-multiplexer, causing each frame to be shredded into suitably small blocks before being reassembled by the recipient.


## Bidirectionality
Unlike bash's unidirectional `|` operator, τ pipelines typically have both a forward and a backward stream. This allows components to locally side-negotiate their communication protocol without a third party to coordinate.
