# φ parsing
It would be useful to have a chunk-merged parser for stuff like websockets. Ideally it doesn't involve unrolling each byte into a separate UTF9, although φ allocation is so fast that it probably isn't the end of the world if we do.

If the input is combinable (e.g. string pieces), then maybe we can simplify by implementing `(state, chunk) → (state', n_consumed, [results...])` like a PEG. In addition to the parse state, it also needs to hold data, e.g. websocket mask bytes, transfer encoding, or remaining content length. So our actual parse function would go like this:

```
data... state chunk start → data'... state' n_consumed [results...]
```

`[results...]` are written to the output φ immediately.
