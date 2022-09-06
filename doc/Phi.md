# Φ
The IO boundary of a process; on Linux, this is the `epoll` event loop. On a browser, Φ is a single worker thread. Φ is the largest area within which heap references can be sent, and all λs within Φ are guaranteed to run in the same thread -- that is, local atomicity is guaranteed.

**Φ is unaware of γ.**
