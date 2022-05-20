# Fabric
The IO mediation layer and CPU scheduler for τ.


## Nonblocking/evented IO
UNIX multiplexes processes over IO devices (and each other), effectively turning each running program into a coroutine. Similarly, τ also needs to manage a loop of virtual "processes", each of which has specific IO channels that produce events that wake these processes up.

In the ideal case, every language that supports async IO can also support τ IO. That might happen with coroutines, futures, callbacks, or just `epoll`; either way, τ presents as an event loop when defining FFIs.


## C++ coroutines
Let's explore this option. I'm starting with [this tutorial](https://www.scs.stanford.edu/~dm/blog/c++-coroutines.html) to figure out the basic landscape.

`g++` 9.4, shipped with Ubuntu 20.04, doesn't have coroutine support yet. Time to upgrade to 22.04 LTS I guess.
