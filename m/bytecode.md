# Bytecode
Is just stream operations, like `ni` but encoded as virtual tasks (C++ classes) with virtual stream objects. Stream objects produce and consume _frames,_ which can be one or more records each. If multiple, batching applies.

Frames are sent via async blocking queues that can be queried. That way write operations are usually async, but will block if we use too much memory.

I think it's fine to use a delegation table and class instances with a `virtual operator()` to execute the bytecode. The `virtual operator()` allows _instantiated_ bytecodes to statically specialize (i.e. rewrite themselves) before execution, which gives us ways to optimize.
