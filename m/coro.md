# Coro
A lightweight API around `boost::context` and `emscripten_fiber_t`. The goal is to just create `coro<T>("name", [&]() -> T {...})` and present a simple API surface to use that coroutine.

Coros shouldn't throw errors from their functions; if we need to capture that, `T` should contain duality. (In practice, we'll probably have `T = int` or something, then just return a number. Most coros are stream processors.)


## Instrumentation
Coros are time-instrumented for a couple of reasons:

1. To enable profiling
2. To allow the [leaf](leaf.md) scheduler to make smart decisions about which coros can be slotted in before the next hard deadline
