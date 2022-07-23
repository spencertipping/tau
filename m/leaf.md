# Leaf
A leaf is a collection of [cells](cell.md) that can exchange data within the same address space. It interfaces to the [trunk](trunk.md) using a serial, almost always multiplexed, boundary cell called the _stem_.

Computationally, each leaf manages schedules for all of its cells' [coros](coro.md). In that sense the leaf is like a kernel managing multiple coro-processes.

A leaf has a [species](species.md) that determines which cells are supported and which cell is used as the stem interface.


## Scheduler
The leaf scheduler manages the [coros](coro.md) that drive cells, tracking their block states and last runtimes to try to schedule fairly. Deadlines are supported for time-scheduled execution, and the scheduler can take input from `epoll` and other external structures.
