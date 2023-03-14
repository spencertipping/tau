# Multiplexers and duplex connections
Static multiplexers are straightforward because they back into Ξ channels:

```
{         # { is multiplexer op
  a[...]  # a[...] is a specific branch
  b[...]  # b[...] is another branch
```

At this point our Ξ will have two named full-duplex channels: `a` and `b`. We can use `}` to demultiplex those down to a single full-duplex tagged connection:

```
{ a[...] b[...] }
```

We could also address the multiplexed Ξ individually prior to the demultiplex, possibly renaming our channels:

```
{ a[...] b[...] .a[...] .b[...] }
```


## Dynamic `()`
Dynamic multiplexers create new branches in response to new input tags. These are written `(π[Γ])`, for example:

```
( >$y )  # write each stream into a file
```

Because `(` is dynamic, **it does not result in a multiplexed Ξ** -- we must immediately demultiplex the stream down to a single full duplex with `)`. Either end can create inner connections.


## Asymmetric cross-multiplexing
The use case here is _mixing:_ for example, we have inputs of the form `(user, document, edit)` and we want to multiplex on documents and publish updates to users. Let's define a complete system around this problem.

First, we have multiple full-duplex user connections, each terminating in an edit-state manager before being mixed to documents.

```
(users) ↔ (edit_state)  # ↔ (user, document, edit)
```

At this point we need to route edits by document, broadcasting to each involved user. Because we're mapping `(user, _)` to `(_, document),` we mix with a connection matrix that implements row/column blocking broadcast/union. This is represented by `'`, which transposes a multiplexing key.

```
(users) (edit_state) ' (document_state) edit_db
```

**Q:** does `'` accept a broadcast Γ?

**TODO:** specify the disconnection protocol for `'`
