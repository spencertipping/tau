# Multiplexers and duplex connections
σ supports two types of multiplexing:

1. Static/heterogeneous, written `{...}`
2. Dynamic/homogeneous, written `(...)`

Static multiplexers are parsed as a series of Γ alternatives, each prefixed by its key. For example:

```
... { a[...] b[...] }
```

There are several things happening here; let's break them down.


## Static multiplexer syntax
`{` and `}` are mirror operators: `{` splits from the left and joins from the right, `}` does the opposite. `{` terminates the top ξ and awaits a series of named-ξ Γ definitions, each of which will transform a keyed axis of the demultiplexed ξ. `}` then joins all keyed things back into the top ξ.

Internally, we do this with scope indexes: `{` increments the scope index and `}` collects+decrements it.

Although you could in principle have an unterminated `{`, it isn't common in practice.


## Dynamic multiplexers
`(` and `)` are also mirrors; unlike `{}`, however, `(` is must always be terminated by a `)`. The only exception is when the inner Γ right-caps its ξ, making the `(` a one-sided operator.

Unlike `(...)`, which is a full-duplex ξ operator, a lone `)` creates a server that tracks tagged inbound connections and mixes them into a single ξ.

**FIXME:** given this design, we should be able to write `( ... &foo` and define `:foo )...`.


## Asymmetric cross-multiplexing
The use case here is _mixing:_ for example, we have inputs of the form `(user, document, edit)` and we want to multiplex on documents and publish updates to users. Let's define a complete system around this problem.

First, we have multiple full-duplex user connections, each terminating in an edit-state manager before being mixed to documents.

```
(users) ↔ (edit_state)  # ↔ (user, document, edit)
```

At this point we need to route edits by document, broadcasting to each involved user. Because we're mapping `(user, _)` to `(_, document),` we mix with a connection matrix that implements row/column blocking broadcast/union. This is represented by `'`, which transposes a multiplexing key.

```
(user (edit_state)) ' (document_state) edit_db
```

For example, suppose `edit_state` is identity and we have this series of inputs from users:

```
(user_1, doc_1, foo)
(user_1, doc_2, bar)
(user_2, doc_3, bif)
(user_2, doc_1, baz)
(user_1, doc_1, bok)
```

`'` would have the following matrix state after each input:

```
foo ⇒ [ (u1 d1) ]
bar ⇒ [ (u1 d1) (u1 d2) ]
bif ⇒ [ (u1 d1) (u1 d2) 0
        0       0       (u2 d3) ]
baz ⇒ [ (u1 d1) (u1 d2) 0
        (u2 d1) 0       (u2 d3) ]
bok ⇒ [ (u1 d1) (u1 d2) 0
        (u2 d1) 0       (u2 d3) ]
```

Let's suppose we're working with the post-`bok` state. There are three types of ηs we might observe:

+ `(user, document, x)`: route directly
+ `(user, τ, x) →`: broadcast across documents
+ `← (document, τ, x)`: broadcast across users

For example, "user 1 disconnected" would be encoded as `(u1, τ, ω)` and would be broadcast to `d1` and `d2`. Similarly, "document 1 was updated" would be `(d1, τ, x)` and would be sent to `u1` and `u2`. More concretely:

```
(u1, τ, ω)              → ' → (d1, u1, ω) (d2, u1, ω)
(u1, d1, x) (u2, d1, x) ← ' ← (d1, τ, x)
```

So the purpose of `'` is twofold: first, we swap the first two tuple elements; and second, we allow _τ_ as a broadcast marker to inform all connected parties.

We can disconnect from `'` using _ω;_ for example, `(u1, d1, ω)` will remove the `(u1 d1)` matrix entry. Similarly, `(u1, d1, α)` will create the matrix entry without sending an edit. Dynamic multiplexers emit _α_ and _ω_ automatically.
