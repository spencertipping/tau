# φ `»`
Parsers are nonlinear element transformations that follow a specific execution pattern. We encode them as [γs](gamma.md) that can be pipelined onto the φs of other γs when building [Γs](Gamma.md).


## Parser structure
It would be useful to have a chunk-merged parser for stuff like websockets. Ideally it doesn't involve unrolling each byte into a separate UTF9, although φ allocation is so fast that it probably isn't the end of the world if we do. Here's a vectorizable parser function signature:

```
data... state input → data'... state' output [results...]
```

`[results...]` are written to the output φ immediately, and the parser is terminated if results cannot be written.


## String parsing
`input` and `output` both take the form `(data, offset)`, where `offset` refers to the next byte of data to be processed. This minimizes the performance impact of small parser movements.
