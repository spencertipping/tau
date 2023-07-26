# Search and traversal
Let's talk about `du`. The simplest way to implement it is recursively:

```cpp
size_t du(std::string path)
{
  size_t r = 0;
  for (auto x : ls(path))
    if (is_file(x)) r += file_size(x);
    else            r += du(path + "/" + x);
  return r;
}
```

Depth-first, _O(d)_ space, and _O(n)_ time. In many cases this is the most efficient strategy, but let's suppose we have a very high-latency hard drive that benefits from deep IO queues. Then we'd want a more breadth-first approach. This would ordinarily require that we completely reorganize the above code, but let's use `future<T>` to avoid having to do that:

```cpp
future<size_t> du(std::string path)
{
  return future([=]()
    {
      size_t r = 0;
      std::vector<future<size_t>> dirs;
      for (auto x : ls(path))
        if (is_file(x)) r += file_size(x);
        else            dirs.push_back(du(path + "/" + x));
      for (auto &x : dirs) r += x.get();
      return r;
    });
}
```

Of course, this is no longer _O(d)_ space; it is much closer to _O(n)_. If we want to control the space complexity, we need to not only limit the number of futures running at any given moment, but we also need to make sure they run in a mostly depth-first order. We could do this by assigning each future a priority corresponding to its degree of nesting, then executing highest-priority futures first. So if `future<>` didn't manage this for us, we'd have:

```cpp
future<size_t> du(int depth, std::string path)
{
  return future(depth, [=]()
    {
      ...
      dirs.push_back(du(depth + 1, ...));
      ...
    });
}
```

`.get()` should begin executing futures with as many threads as we've assigned to future execution. Here's the basic idea:

```cpp
template<class T>
struct future
{
  future(std::function<T()> f)
    : has_result(false)
    { pending.push([this]() { set(f()); }); }

  void set(T r)
  {
    result     = r;
    has_result = true;
  }

  T get()
  {
    // Pretend this uses a thread pool and locking, etc
    while (!pending.empty())
    {
      auto top = pending.top();
      pending.pop();
      top();
      if (has_result) return result;
    }
    // Should never get here
  }

  static std::priority_queue<std::function<void()>, ...> pending;

protected:
  T    result;
  bool has_result;
};
```


## Futures in τ
If we ran `du` on a filesystem with directory hardlinking (so a directory can have multiple parents), then we'd want to deduplicate futures by inode. Since that creates _O(n)_ space usage, we might also want to offload the `future<>` results into a database.

Further, for problems that aren't `du` we might want to distribute the work in some way. So we want to reduce each worker to something that can negotiate futures with a simple ξ IO boundary. Here's the basic idea:

1. `future<>` is replaced by an async-recursive-invocation operator, in this case `#f`
2. `#f` generates a "future" (really just a hash of its args) and returns it; it emits its arguments as a future-execution request, which will be forwarded to the future controller
3. The future controller checks caches and, if appropriate, adds the request to the main priority queue
4. When the worker calls `!` to retrieve one or more futures' values, it emits a future-result request to the future controller (to inform it about how to close out threads)
5. When a thread returns, the return value is emitted with its future key to the future controller, which can then alert whichever worker (if any) was depending on it


## IO
The future controller simply moves values to the correct workers; that's it. So it supports a two-layer key that includes the worker ID and the future ID. Its IO looks like this:

+ _(wid=fid, α, ...) → w_: initialize a worker
+ _(wid, fid, ω, ...) → w_: provide a future's value to a worker
+ _w → (wid, fid, α, ...)_: request a recursive future
+ _w → (wid=fid, ω, ...)_: return a value for a future (from a child)

**NOTE:** futures hold horizontal values; we don't support vertical streams.

**NOTE:** _fid_ is a hash of the future's calling arguments. This guarantees ID stability and makes it possible to cache future results for large computations.

**NOTE:** _wid = fid_ for child workers.


## Search context
`?[...]` is usually combined with `*[]` ([replicated multiplexing](sigma-multiplex.md)) to create a recursive future-evaluation context that splays recursive calls across different worker threads. For example:

```
?*pf
```
