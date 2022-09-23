# π performance
π needs to be roughly as fast as Python or Ruby. UTF9 already has native support for NumPy-style tensors and numeric vectors, which will be handled in native code as they are in NumPy. Most of the remaining performance considerations come down to how quickly values can be moved across φs or individually transformed (i.e. we have both latency and throughput requirements).

Because interpreters have some overhead, the goal is to minimize the number of bytecodes required to do a given task. This is likely to be an iterative optimization effort, so it's important that the bytecode set be easy to modify and that bytecodes themselves be easy to write. A function table with symbolic assembler names is probably fine here.
