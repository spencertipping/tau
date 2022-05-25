# Fabric
The IO mediation layer and CPU scheduler for τ. An instance of fabric is called a `node` and represents the basic data boundary within τ. Operators can either be _inlined_ into the same node, or they can multiplex their outputs across IPC into other nodes.


## Nonblocking/evented IO
UNIX multiplexes processes over IO devices (and each other), effectively turning each running program into a coroutine. Similarly, τ also needs to manage a loop of virtual "processes", each of which has specific IO channels that produce events that wake these processes up.

I took a quick look at SysV IPC mechanisms, and the general preference seems to be to use FIFOs/UNIX domain sockets rather than trying to get shared memory and shared semaphores to work out. Makes sense to me; I'd rather avoid creating a global resource if we can, and `epoll` works only with POSIX, not SysV, message queues.

...so if we want a multi-language environment that uses nonblocking IO as its τ boundary, we basically merge the system's event loop with our own FIFO/socket demultiplexer that activates specific coroutines when IO is ready. That is, we establish an event sub-loop triggered by the main event loop.

We should probably have each thread in its own process to minimize complexity. That way they're all just remote components.

**NOTE:** let's simplify the transfer model by excluding shared memory altogether. This means `epoll` will work correctly for messages; we don't need any queueing silliness. So within a node (== thread) we have inlined operators; otherwise we send stuff down FIFOs.

**NOTE:** boundary IO operators should `close` file descriptors after sending them via UNIX socket (and should hard-fail if we're trying to send FDs over other types of connections).

**Q:** do we actually use `epoll` for IPC, or do we use soft negotiation? Perhaps both.


## C++ coroutines
Let's explore this option. I'm starting with [this tutorial](https://www.scs.stanford.edu/~dm/blog/c++-coroutines.html) to figure out the basic landscape.

`g++` 9.4, shipped with Ubuntu 20.04, doesn't have coroutine support yet. 22.04, with `g++` 11.2, has what we need.

[Generally useful thread about Boost C++ libraries](https://www.reddit.com/r/cpp/comments/jn72ol/what_are_you_most_used_boost_libraries/)


### Stackless coroutines
C++ coroutines are stackless, meaning that stack context isn't saved when you `co_await` or `co_yield` out of them. In particular, this means we can't have library functions that implement `co_await` to automate suspension. That logic needs to be inlined directly into the tasks that are running, which seems like a problem.

We can sidestep all of this by writing our own AMD64 assembly to switch out stacks and flush locals; then we manage machine-code threads and allocate our own stacks.


### `Boost.Fiber`
+ [Some HN discussion of C++ fibers](https://news.ycombinator.com/item?id=21229082)
+ [`boost::fiber` tutorial](https://www.romange.com/2018/12/15/introduction-to-fibers-in-c-/)
+ [`boost::fiber` performance measurements](https://www.boost.org/doc/libs/1_67_0/libs/fiber/doc/html/fiber/performance.html)

50-90ns per context-switch is quite good, 5-10x faster than Go or Erlang. Performance becomes even less of an issue if we do a couple more things:

1. Inline 1:1 operations (i.e. no input or output loops)
2. Use directed linking to bypass the main queue for internal links
