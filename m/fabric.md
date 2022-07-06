# Fabric




# TODO: rewrite stuff below
The IO mediation layer and CPU scheduler for τ. An instance of fabric is called a `node` and represents the basic data boundary within τ. Operators can either be _inlined_ into the same node, or they can multiplex their outputs across IPC into other nodes.


## Fundamental components
1. `utf9` frames (record containers)
2. `utf9` frame streams
3. Byte streams (only for boundary IO)
4. `epoll` IO boundary container
5. Task/continuation scheduler
6. Boot operator + configuration schema
7. Operator-level rewindable memory allocator

**NOTE:** memory allocation isn't strictly a fabric concern; I think I can push that into operator and stream implementations. We can start with standard `new` and `delete` and get more elaborate from there.

**NOTE:** boundary IO operators should `close` file descriptors after sending them via UNIX socket (and should hard-fail if we're trying to send FDs over other types of connections).


## Dynamic fabric
Each node has an _n_-ary "fabric modification operator" that accepts records that modify the connection topology.

**Q:** what's the core data structure here?

**Q:** is there any automatic memory management -- i.e. do abandoned operators automatically get GC'd somehow? (Sure, it's possible they do if individual clients are multiplexed and use _τ_ to punctuate atomic changes.)


## Scheduler logic
A few high-level considerations:

1. The set of streams is variable during runtime (e.g. with multiplexers)
2. The set of operators is also variable
3. All IO boundaries are FFI'd with first-class operators that adapt to fabric flow control
4. Each stream has two ends, the "read end" and the "write end"
5. Each end of each stream is connected to at most one operator
6. We will often have chained operators for which _ι_ propagates linearly

(1) and (2) mean that we can't rely on expensive static/topological analysis. (5) means that we don't have the "broadcast readiness" event problem; readiness-flow is strictly deterministic. (6) hints that we should be good at serial chains, which happens trivially if we have direct lookups.

(6) is also subject to latency constraints. That is, if forward-latency is bounded by something aggressive like 10μs, we'll have a lot of fiber switching. If we have a longer latency, stream capacitance will take up more slack and allow each operator to run for longer.

The simplest approach is just to have an `unordered_map<endpoint_id, operator>` that we consult for each _ι_ event. Operators can maintain bitmasks of operand readiness and activate when sufficient readiness occurs. Everything remains edge-triggered and fast.

I think streams are themselves operators that just move data from one endpoint to the other. This way they can define their own capacitance.


### Operator chains
...probably shouldn't exist from the fabric's perspective. Each operator would need to read, decode, process, encode, and send each record -- which is a lot of unnecessary overhead. Any 1:1 operator should act against a proxy object that can quickly stage edits.

We should also define fairly generous cell-operator grammars that allow us to pack a lot of functionality into a single operator.


### Enumeration: a quick example
Suppose we have the `n` operator that generates an integer sequence. So `n` by itself produces integers indefinitely, whereas `n10` produces just `0..9`. We have another operator that prints these integers to `stdout`.

```
n   stdout   # <- program 1
n10 stdout   # <- program 2
```

Internally, `n` could be written like this (note that I'm leaving a lot of abstraction on the table to demonstrate the internals):

```cpp
class n : public op
{
  uint64_t         limit;
  stream<uint64_t> out;
  virtual void operator()()
  {
    for (uint64_t i = 0; out.open() && (!limit || i < limit); ++i)
      out << i;               // assume uint64_t is coercible to record
    out.omega();              // close the stream
  }
};

template <typename T>
class stdout : public op      // note: not a real operator
{
  stream<T> in;
  virtual void operator()()
  {
    T x;
    while (in.open())
    {
      in >> x;
      cout << x;
    }
    in.omega();
  }
};
```

`stream::operator<<`, `stream::operator>>`, and `stream::omega` are the continuation scheduling points here:

+ `stream::operator<<` inserts an item, yielding if the stream is at capacity
+ `stream::operator>>` removes an item, yielding if the stream is empty
+ `stream::omega` on an outbound stream schedules the other side

1:1 operators can be written more simply:

```cpp
template <typename T, typename U>
class direct_push_op : public op
{
  stream<U> out;
  virtual void operator<<(T &x) const
  {
    out << f(x);              // for some f()
  }
};
```


## Performance-adjacent issues
### Compilation model
Templated operators require a pass through C++ to produce a custom binary, whereas we can use RTTI-wrapped types if we want a generic binary instead. If we want to avoid the `g++` overhead for short runs, we can start the program with RTTI generics and compile a specialized version, then do a fabric handoff.


### `boost::context` continuation performance
I just wrote [a benchmark](../dev/hackery/continuation-perf.cc) that takes 5.9s to execute 1B full-loop context switches (that is, switch into task, do something, switch back to main). That gives us an average of 5.9ns per full-loop switch.

That's stupidly fast, holy hell. Is `g++` optimizing the continuations out of the picture?


### Multithreading
I was originally thinking we'd try to make a multithreaded scheduler, but I don't think it's important. We can have each τ fabric be single-threaded and create more IPC-connected ones to get multicore parallelism. That way there's no difference between "cores on one machine" and "cores on other machines"; they're all separated by an IPC barrier.

**Q:** how do we deal with bottlenecks that could be parallelized? I think we want fabric-affinity to be inferred from topology rather than explicitly specified.


### Latency control
I'm thinking about the case where we want to schedule absolute-time events with high accuracy. Step 1 is for the fabric to be able to abort an operator when it attempts IO, even before its IO constraints would normally yield the fiber.

Step 2, if we want accuracy, is for the fabric to systematically measure the latency distribution of each operator so it doesn't overcommit to a non-preemptible chunk of work that will overlap the scheduled time point. (Relevant when we have CPU-intensive steps.)
