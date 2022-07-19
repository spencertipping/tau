# Axis
An axis is a domain within which all angular values can be meaningfully compared -- that is, time is fully ordered. In practice, this means that an axis is a single UNIX or WASM process that manages a set of cooperatively-threaded coroutines.

UNIX axes are bounded by `epoll`, WASM by the browser's event loop. Because the trigger mechanisms differ, they are managed externally, although the axis can help by tying specific event sources to their corresponding coros.


## Orbit
An axis is the system as a whole; its individual components are called _orbits_ and they represent logical tasks that move data. An orbit can define multiple coros, each of which will "block" (yield) on IO operations.


## Channels
Channels carry values between orbits, yielding automatically when saturated. Functionally, a channel is a bounded queue that will accept up to _n_ objects and allow them to be durably retrieved in the order sent. The queue will block, causing a coro to yield, if read while empty or written to while full.

**TODO:** define how channels are sized -- this should be done by the axis
