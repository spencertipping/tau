# Φ
The IO boundary of a process; on Linux, this is the `epoll` event loop. On a browser, Φ is a single worker thread. Φ is bounded by a single address space, and all λs within Φ are guaranteed to run in the same thread -- that is, linear atomicity is guaranteed.

Typically, a Φ will host one or more [π₀](pi0.md) interpreters.

**Φ is unaware of γ.**
