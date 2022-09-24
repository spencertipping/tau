# π performance
π needs to be roughly as fast as Python or Ruby. UTF9 already has native support for NumPy-style tensors and numeric vectors, which will be handled in native code as they are in NumPy. Most of the remaining performance considerations come down to how quickly values can be moved across φs or individually transformed (i.e. we have both latency and throughput requirements).

Because interpreters have some overhead, the goal is to minimize the number of bytecodes required to do a given task. This is likely to be an iterative optimization effort, so it's important that the bytecode set be easy to modify and that bytecodes themselves be easy to write. A function table with symbolic assembler names is fine here.


## Correlated prediction
Python's bytecode interpreter has a clever hack that reflects the conditional distribution of bytecodes: after a `compare`, for instance, it will do a couple of non-table checks to see whether the next instruction is a jump -- because those are common after `compare`, the interpreter saves a `switch()` most of the time. (See [this article](https://realpython.com/cpython-source-code-guide/#execution) for details.)

Quoting the article:

> Opcode predictions: Some opcodes tend to come in pairs thus making it possible to predict the second code when the first is run. For example, `COMPARE_OP` is often followed by `POP_JUMP_IF_FALSE` or `POP_JUMP_IF_TRUE`.
>
> “Verifying the prediction costs a single high-speed test of a register variable against a constant. If the pairing was good, then the processor’s own internal branch predication has a high likelihood of success, resulting in a nearly zero-overhead transition to the next opcode. A successful prediction saves a trip through the eval-loop including its unpredictable switch-case branch. Combined with the processor’s internal branch prediction, a successful `PREDICT` has the effect of making the two opcodes run as if they were a single new opcode with the bodies combined.”
>
> If collecting opcode statistics, you have two choices:
>
> 1. Keep the predictions turned-on and interpret the results as if some opcodes had been combined
> 2. Turn off predictions so that the opcode frequency counter updates for both opcodes
>
> Opcode prediction is disabled with threaded code since the latter allows the CPU to record separate branch prediction information for each opcode.

This is both smart and misguided: although they're correct to exploit the conditional distribution of bytecodes, the bytecodes should probably be less correlated in the first place.

π can work together with [∏](Pi.md) to profile the conditional distribution and create fused bytecodes, in effect Huffman-encoding the interpreter (although without the attendant decoding complexity, since bytecodes are fixed-width). In other words, the size and to some extent encoding of bytecodes need not be concretely-specified. In order to make this work, π's bytecode implementations are _source-concatenative_ as well as _execution-concatenative_ -- that is, appending the source is the same as appending the bytecodes, just as would be true in Joy.

In other words, although π has some hand-defined bytecodes, much of what runs in production will be created by π itself. Because the π bytecode set is AOT compiled, we need an initial set of programs to profile. Luckily this profiling process doesn't involve much work; we can scan forwards without interpreting anything. Then we build out a Huffman tree and select up to _n_ bytecode paths to pre-fuse. This tree is encoded into the ∏ assembler as a series of macro-like substitutions.
