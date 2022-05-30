# Stream semantics
Durable, ordered, flow-negotiated. We can do sequence-acking if we want durability over distributed systems, but we'll need commit barriers and replay buffers. Those are extra, as is bidirectionality. At their core, streams are just atomic conveyors of [records](records.md).

Each stream can specify its "maximum atomic write" capacity, which must be at least 512 bytes. This makes it compatible with even the lowest values of `PIPE_BUF` on UNIX systems. Typically, a stream that offers very low atomic-write capacity will be wrapped with a byte-multiplexer, causing each record to be shredded into suitably small blocks before being reassembled by the recipient.


## Bidirectionality
Unlike bash's unidirectional `|` operator, τ pipelines typically have both a forward and a backward stream. This allows components to locally side-negotiate their communication protocol without a third party to coordinate.


## Flow negotiation
Flow is negotiated by fabric, which tracks stream-ready states and continuation scheduling. The boundary IO `epoll` _informs_ stream states, but is not the primary negotiation mechanism.

Fixed-arity operators define a bitfield of channel-readiness that is updated by the fabric. They can also schedule continuations against that bitmask. This is useful for situations like cut-through mappers, which run only when input and output are both ready.

_τ_ provides for _opportunistic multiplexed flow optimization;_ that is, its flow-state reporting is much more granular than "this channel is ready for data". The `IOTA` stream operator, which is an edge trigger that means "this stream has become ready for data", can be serialized into a utf9 record and forwarded, e.g. to a multiplexing peer. Then the peer can sometimes forward data even if not all demultiplexer-side channels are ready. In other words, flow state can be modeled in arbitrary fidelity by any operator (up to reference frame consistency).


## Readiness for reading and writing
For record-carrying channels, "ready" means that at least one record can be either read or written. IO boundaries impose limits on the size of record that can be written without issue.

Data commitment is internally mediated by reverse-channel acknowledgement, usually working backwards to the most recent replay point in a pipeline. Because this is all custom protocol, τ has no built-in symbol to convey that data has been written successfully.


## Errors
τ has no built-in error handling. If operators are expected to produce errors as part of production operation, they should be multiplexed so they have a control channel that can convey these cases.
