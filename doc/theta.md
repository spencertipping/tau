# Θ
Deadline scheduling is an interesting problem because Λ is cooperatively-threaded, meaning that we can't interrupt a process. That means our time quanta have non-constant size, which requires us to measure their distribution and make a calculated guess about how much work we can schedule before the next deadline occurs.

Θ does this by using a stopwatch per coro; it is informed whenever Λ jumps into or out of a coro, snapshotting the time into a log-histogram.

```cpp
struct Θ
{
  M<λi, Θw> λΘ;
  λi        cur;


};
```
