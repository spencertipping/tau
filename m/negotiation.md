# Stream negotiation and partial IO
Streams should have some size preference about the messages they produce and accept. I imagine this should be global and based on the cache.

This matters because we may have bulk-consumers that do vectorized operations; if so, then we risk transposing the execution order if we force strict evaluation. This means we need to have our bulk executors produce and consume _streams,_ not buffers, making them interruptible. It's streaming all the way down.

**NOTE:** auto-vectorization is unnecessary. We can use APL-style array embedding to get implicit per-op broadcast, and we can gather row groups up if we want buffered vectorization.
