# Tau
Main components:

1. Multi-threaded native fabric with boundary `epoll` IO (single `epoll` proxy thread, probably)
2. Multi-worker wasm fabric
3. Fabric configuration directives
4. Source parser

Multithreaded native processes will have one IO-boundary process and all others will be simple workers using utf9 messaging within shared memory to communicate.

**Q:** what type of GC is appropriate?
