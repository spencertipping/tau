# Tau
+ [Stream operation bytecode](m/bytecode.md)
+ [Stream semantics](m/stream.md)
+ [Negotiation and IO](m/negotiation.md)
+ [Frames and records](m/records.md)
+ [C++ API](m/cpp.md)


## Main components
1. Multi-threaded native fabric with boundary `epoll` IO (single `epoll` proxy thread, probably)
2. Multi-worker wasm fabric
3. Fabric configuration directives

Multithreaded native processes will have one IO-boundary process and all others will be simple workers using utf9 messaging within shared memory to communicate.

**Q:** what type of GC is appropriate?

**Q:** what is our core execution bytecode?

**NOTE:** both execution and pipeline sections can be parsed with PEGs
