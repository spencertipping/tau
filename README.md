# Tau
Main components:

1. Multi-threaded native fabric with boundary `epoll` IO (single `epoll` proxy thread, probably)
2. Multi-worker wasm fabric
3. Fabric configuration directives
4. Source parser

Multithreaded native processes will have one IO-boundary process and all others will be simple workers using utf9 messaging within shared memory to communicate.

**Q:** what type of GC is appropriate?

**Q:** what is our core execution bytecode?

**NOTE:** both execution and pipeline sections can be parsed with PEGs


## Bytecode
Is just stream operations, like `ni` but encoded as virtual tasks (C++ classes) with virtual stream objects. Stream objects produce and consume _frames,_ which can be one or more records each. If multiple, batching applies.


## Stream semantics
Durable, ordered. We can do sequence-acking if we want durability over distributed systems, but we'll need commit barriers and replay buffers.

Most connections are bidirectional: `A-->B` is actually `A===B`, where `B` can "reply" to `A`.


## Stream negotiation and partial IO
Streams should have some size preference about the messages they produce and accept. I imagine this should be global and based on the cache.

This matters because we may have bulk-consumers that do vectorized operations; if so, then we risk transposing the execution order if we force strict evaluation. This means we need to have our bulk executors produce and consume _streams,_ not buffers, making them interruptible. It's streaming all the way down.

**NOTE:** auto-vectorization is unnecessary. We can use APL-style array embedding to get implicit per-op broadcast, and we can gather row groups up if we want buffered vectorization.
