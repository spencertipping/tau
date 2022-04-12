# Tau
bash + APL + lisp, powered by a C++ dataflow routing fabric. Pipelines are built directionally with direct polymorphic consing; the fabric receives the source as its streaming input and uses invertible parse lenses to interpret it. This requires the fabric to refer to a document model.

Streams and endpoints have names that can be sent down other streams. The fabric will dereference these and implement connection operations.

We also need a connection/disconnection protocol; this should be a control channel that receives new FDs as data (and another control message to disconnect). Components manage their own connections this way, but the fabric manages broker-forwarding.

Connection cursors are bundles of streams; the bundle contents and lens context influence polymorphic operator selection.


## Null-space ambiguity resolution
The user most likely intended the interpretation with the smallest null space. I think equivalently, if there's an equivalent and much shorter way to say what the user meant, then they probably intended something else.

Thus DWIM is a function of entropy.


## Canvas dimensionality
**Q:** should we have a 2D source canvas given that fabric instructions are serial? (Or are they serial? Also, let's consider side-channel information and forking. Maybe DAGs are the fundamental data structure.)

```
φ /pub <static
  /api %@auth ]dh : ot d <> $"ot/d"
                    ping ...
```

**TODO:** convert the above to be from the global perspective, not just webserver (i.e. download code to the client)


## API generation
**TODO:** scan github for usages of various APIs and auto-convert those to tau idioms
