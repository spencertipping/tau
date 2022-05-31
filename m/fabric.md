# Fabric
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


## Scheduler logic
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
