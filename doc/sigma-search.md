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
The future controller simply moves values to the correct workers; that's it. So it supports a two-layer key that includes the worker ID _w_ and the future ID _f_. Its IO looks like this:

+ _w ... →_: create a worker
+ _w ω f ... →_: inform a worker about a future's value
+ _→ w α f ..._: worker creates a future
+ _→ w ..._: worker returns a value for a future

**NOTE:** futures hold horizontal values; we don't support vertical streams.

**NOTE:** _f_ is a hash of the future's calling arguments. This guarantees ID stability and makes it possible to cache future results for large computations.

**NOTE:** _w = f_ for child workers.


## Fanout control
This isn't obvious to me right now, so let's figure it out.

If we approach a problem depth-first, we'll end up using _O(d)_ stack frames at a minimum -- and if each is encoded as a future, then we'll have _O(d)_ futures. Since _d_ isn't usually bounded by any specific number, this means we can't impose a limit on the number of parallel evaluations going on.

The problem seems to be that we have `!f` and `#f` happening in the same π process, so we have to keep the interpreter in memory. Can't do futures without continuations, can't do disk-persistent continuations without serializable heap+source+call points. We're actually pretty close there, so maybe it would work?

**Option 1:** add disk-persistent continuations to π

**Option 2:** use separate π processes for `#f` and `!f`: scatter/gather

Of these two options, (2) is a lot simpler. How would `du` work?

```
:du ? @:N p -dx ? *#f ls x : -s x;
          p x<| +/ x|>
```

`*#f` fans out, `-s` computes inline size, `x<| +/ x|>` emits `path size` pairs from `path size...` aggregates.


## Search context
**FIXME:** how do we manage flow control? This is a big question.

`?` is usually combined with `*[]` ([replicated multiplexing](sigma-multiplex.md)) to create a recursive future-evaluation context that splays recursive calls across different worker threads. For example:

```
:du ? @=N @:N *p -dx ? +/ *!f *#f ls x
                     : -s x
```

Equivalently: `:du?-@:N*p-dx?+/*!f*#flsx:-sx`.

Let's break this down:

+ `? q c f` is a future controller with queue `q`, cache `c`, and function `f`
+ `@=N` is a FIFO queue, which results in depth-first evaluation
+ `@:N` is a native C++ map
+ `*[]` is a replicated multiplex
+ `p...` is π
+ `-d` is Perl's `-d`, `-dx` means "is it a dir"
+ `?:` is ternary conditional
+ `+/` sums a list
+ `*!f ∷ [F<T>] → [T]`
+ `*#f ∷ [T] → [F<f(T)>]`
+ `ls x` = directory entries
+ `-s x` = file size (same as Perl)

Here's the IO, e.g. for `i"/" $du`, where `H[]` is the arg-hashing function (SHA of η). `→` is a horizontal record, `⇒` is a vertical stream (so `(1 2 3) ⇒` is equivalent to `1 2 3 →`):

+ `"/" → ? c f`
  + `H["/"] ι → c`
  + `c → H["/"] ω`: a cache miss
+ `H["/"] "/" → f`: create a new π
  + `"/" → p...`: initialize the π; `x = "/"`
  + `-d "/" = true`
    + `ls x = "/bin" "/etc" ...`
    + `*#f "/bin" "/etc" ... = H["/bin"] H["/etc"] ... ⇒ (α H["/bin"] "/bin") (α H["/etc"] "/etc") ...`
    + `⇒ (H["/"] α H["/bin"] "/bin") (H["/"] α H["/etc"] "/etc") ...` due to `*`

At this point `?` is back in play: it sees this stream of requests and stores them into the pending-future cache, which is keyed on `H[x]` and stores one of these mappings:

```
H[x] → ι f(x)  # if f(x) is known
H[x] → α w x   # if it's queued but not yet known
H[x] → ω       # if we've never seen x before
```

Note that the cache doesn't have a "running" state; if we did, it would be possible for the cache to desynchronize from reality, e.g. if the program is interrupted and the cache is disk-based. As it is, the worst case is that we have extra queued records that correspond to futures whose values will never be requested.

Anyway, `? → c` is the following: `(H["/bin"] ι) (H["/etc"] ι) ...`; we then observe `(H["/bin"] ω) (H["/etc"] ω) ...`, so we store `α` records; `? → c` is:

```
H["/bin"] α α H["/"] "/bin"
H["/etc"] α α H["/"] "/etc"
...
```

Each of these is also enqueued: `? → q` is `(H["/bin"] α) (H["/etc"] α)`.
