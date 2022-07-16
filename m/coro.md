# Coro
A lightweight API around `boost::context` and `emscripten_fiber_t`. The goal is to just create `coro("name", [&]() {...})` and present a simple API surface to use that coroutine.

Something like this:

```cpp
template<class T>
struct coro<T>
{
  template<class fn> coro(string name, fn);

  bool done() const noexcept;

  coro &operator()();   // transfer control to this coro
  static void yield();  // yield out to main coro
};

// must call this before using tau::coro
void coro_init();
```

Coros shouldn't throw errors from their functions; if we need to capture that, `T` should contain duality. (In practice, we'll probably have `T = int` or something, then just return a number. Most coros are stream processors.)

**NOTE:** `coro<T>` may present a challenge if we want to have `this_coro`. Maybe we should limit it to an externally-defined main context or have `coro` objects not return values.
