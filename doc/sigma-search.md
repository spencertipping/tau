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

Of course, this is no longer _O(d)_ space; it is much closer to _O(n)_. If we want to control the space complexity, we need to not only limit the number of futures running at any given moment, but we also need to make sure they run in a depth-first order. We could do this by assigning each future a priority corresponding to its degree of nesting, then executing highest-priority futures first. So if `future<>` didn't manage this for us, we'd have:

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

`.get()` should begin executing futures with as many threads as we've assigned to future execution. The single-threaded version would look like this:

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
    while (!pending.empty())
    {
      // Pretend this is thread-safe
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
