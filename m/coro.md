# Coro
A lightweight API around `boost::context` and `emscripten_fiber_t`. The goal is to just create `coro("name", [&]() {...})` and present a simple API surface to use that coroutine.

Something like this:

```cpp
struct coro
{
  template<class fn>
  coro(string name, fn);

  coro &operator()();
  void finish();

  static coro &main();
  static coro &current();
  static void  fin();
};

void coro_init();
```


## Results and errors
We should probably have a way to capture exceptions and/or return values. Maybe we use `coro<T>` to forward returns and errors, which I guess would both be encapsulated by `T`.

This also provides more natural `return` support within `coro` lambdas.
