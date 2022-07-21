# Axis
An axis is a domain within which all angular values can be meaningfully compared -- that is, time is fully ordered. In practice, this means that an axis is a single UNIX or WASM process that manages a set of cooperatively-threaded coroutines.

UNIX axes are bounded by `epoll`, WASM by the browser's event loop. Because the trigger mechanisms differ, they are managed externally, although the axis can help by tying specific event sources to their corresponding coros.


## Port
A port is an entry or exit point for UTF9 values. Ports are yield-blocking in the sense that they must be in a ready-state for operations against them not to yield out to the main axis scheduler.

Ports are viewed in two different ways. The axis machinery views ports as endpoints to conduits, which forward data from one port to another with internal queueing. This is the "kernel view" of a port.

Rings see ports as lightweight accessor objects that yield to axis machinery when operations should block.


## Ring
Rings are logical tasks that involve one or more coros and one or more ports. A ring can create and destroy coros as it runs, sort of like a UNIX process can create and destroy threads.
