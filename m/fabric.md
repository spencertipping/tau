# Fabric
The IO mediation layer and CPU scheduler for τ. An instance of fabric is called a `node` and represents the basic data boundary within τ. Operators can either be _inlined_ into the same node, or they can multiplex their outputs across IPC into other nodes.


## Fundamental components
1. `utf9` frames (record containers)
2. `utf9` frame streams
3. Byte streams
4. `epoll` IO boundary container
5. Task/continuation scheduler
6. Boot operator + configuration schema
7. Operator-level rewindable memory allocator

**NOTE:** memory allocation isn't strictly a fabric concern; I think I can push that into operator and stream implementations.


## Nonblocking/evented IO
UNIX multiplexes processes over IO devices (and each other), effectively turning each running program into a coroutine. Similarly, τ also needs to manage a loop of virtual "processes", each of which has specific IO channels that produce events that wake these processes up.

I took a quick look at SysV IPC mechanisms, and the general preference seems to be to use FIFOs/UNIX domain sockets rather than trying to get shared memory and shared semaphores to work out. Makes sense to me; I'd rather avoid creating a global resource if we can, and `epoll` works only with POSIX, not SysV, message queues.

...so if we want a multi-language environment that uses nonblocking IO as its τ boundary, we basically merge the system's event loop with our own FIFO/socket demultiplexer that activates specific coroutines when IO is ready. That is, we establish an event sub-loop triggered by the main event loop.

We should probably have each thread in its own process to minimize complexity. That way they're all just remote components.

**NOTE:** let's simplify the transfer model by excluding shared memory altogether. This means `epoll` will work correctly for messages; we don't need any queueing silliness. So within a node (== thread) we have inlined operators; otherwise we send stuff down FIFOs.

**NOTE:** boundary IO operators should `close` file descriptors after sending them via UNIX socket (and should hard-fail if we're trying to send FDs over other types of connections).

**Q:** do we actually use `epoll` for IPC, or do we use soft negotiation? Perhaps both.


## Fabric/stream soft mediation
Suppose we have preallocated buffers (probably custom `std::queue` implementations) to forward messages between operators. Then we predicate control transfer on a mixture of space and time:

```cpp
void operator<<(stream<u9> &to, u9 &r) noexcept
{
  while (to.size() && (!to.can_store(r) || to.latency_expired()))
    // TODO: set this-fiber state
    main = std::move(main).resume();
  to.push_back(r);
}
```

Ideally we have a bit more logic in our fiber-selection loop to know which fibers can be resumed at which moments. Most are likely to be bottlenecked on a single queue at any given point, although perhaps we can autoscale those sections, possibly about _τ_ boundaries.

Note that in practice we'll write operators as objects that maintain `this_fiber` correctly, so we yield directly to the main fiber rather than using a task selection loop.

**Important use case:** we'll have some "impedance matching" components that do stuff like consume input at full speed, forwarding or discarding all of it, but maintaining an output of sampled elements (random or priority-queued) that is consumed at a different rate. This means we need non-blocking ways to measure IO impedance and throughput: "is this channel ready" type of logic.

We may also want "notify when any of these channels are ready", which means there's no single blocking point. That's easy enough; we just need an indirection between channel-readiness and the subscribed event.


### Scheduler logic
A few high-level considerations:

1. The set of streams is variable during runtime (e.g. with multiplexers)
2. The set of operators is also variable
3. All IO boundaries are FFI'd with first-class operators that adapt to fabric flow control
4. Each stream has two ends, the "read end" and the "write end"
5. Each end of each stream is connected to at most one operator
6. We will often have chained operators for which _ι_ propagates linearly

(1) and (2) mean that we can't rely on expensive static/topological analysis. (5) means that we don't have the "broadcast readiness" event problem; readiness-flow is strictly deterministic. (6) hints that we should be good at serial chains, which happens trivially if we have direct lookups.

The simplest approach is just to have an `unordered_map<endpoint_id, operator>` that we consult for each _ι_ event. Operators can maintain bitmasks of operand readiness and activate when sufficient readiness occurs. Everything remains edge-triggered and fast.

I think streams are themselves operators that just move data from one endpoint to the other. This way they can define their own capacitance.


## Performance-adjacent issues
### `boost::context` continuation performance
I just wrote [a benchmark](../dev/hackery/continuation-perf.cc) that takes 5.9s to execute 1B full-loop context switches (that is, switch into task, do something, switch back to main). That gives us an average of 5.9ns per full-loop switch.

That's stupidly fast, holy hell. Is `g++` optimizing the continuations out of the picture?


### Multithreading
I was originally thinking we'd try to make a multithreaded scheduler, but I don't think it's important. We can have each τ fabric be single-threaded and create more IPC-connected ones to get multicore parallelism. That way there's no difference between "cores on one machine" and "cores on other machines"; they're all separated by an IPC barrier.

**Q:** how do we deal with bottlenecks that could be parallelized? I think we want fabric-affinity to be inferred from topology rather than explicitly specified.


### Latency control
I'm thinking about the case where we want to schedule absolute-time events with high accuracy. Step 1 is for the fabric to be able to abort an operator when it attempts IO, even before its IO constraints would normally yield the fiber.

Step 2, if we want accuracy, is for the fabric to systematically measure the latency distribution of each operator so it doesn't overcommit to a non-preemptible chunk of work that will overlap the scheduled time point. (Relevant when we have CPU-intensive steps.)
