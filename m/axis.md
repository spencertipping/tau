# Axis
An axis is a domain within which all angular values can be meaningfully compared -- that is, time is fully ordered. In practice, this means that an axis is a single UNIX or WASM process that manages a set of cooperatively-threaded coroutines.

UNIX axes are bounded by `epoll`, WASM by the browser's event loop. Because the trigger mechanisms differ, they are managed externally, although the axis can help by tying specific event sources to their corresponding coros.


## Orbits
A coro specialized to move data within an axis is called an _orbit_.
