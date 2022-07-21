# Coro
A lightweight API around `boost::context` and `emscripten_fiber_t`. The goal is to just create `coro<T>("name", [&]() -> T {...})` and present a simple API surface to use that coroutine.

Coros shouldn't throw errors from their functions; if we need to capture that, `T` should contain duality. (In practice, we'll probably have `T = int` or something, then just return a number. Most coros are stream processors.)
