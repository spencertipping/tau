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
Multiplexed ξ alternatives may terminate independently; by default, `{}` will send `k ω` for any `k` that does so. `{}` will fail if you write to an expired alternative.

```bash
$ $sigma 'n1p("a" 1)("b" 2)("a" 3)@-
          K {a|p"foo"x+1; b|p"bar"x-1}' | sort
"a" "foo" 2
"a" "foo" 4
"a" ω
"b" "bar" 1
"b" ω
$ $sigma 'n1p("a" 1)("b" 2)("a" 3)("a" ω)("a" 4)@-
          K {a|p"foo"x+1; b|p"bar"x-1}' | sort
"a" "foo" 2
"a" "foo" 4
"a" ω
"b" "bar" 1
"b" ω
```


## Replicated multiplexers
`*[]` creates a multiplexed group that creates copies of a process. Workers have names and are created using _(w, α, ...)_. Here, _w_ is the worker ID and _..._ is the initial argument forwarded to the worker. `*[]` peels a reference by one layer, so the inner process sees _(α, ...)_; anything the worker sends will have _w_ prepended.

Aside from dropping its input (i.e. exiting), the worker can indicate finality in one of two ways:

1. _(ω, ...)_: send a final message `...` and exit
2. _(τ, ...)_: send a final message `...` and indicate that the worker can be reinitialized with _(α, ...)_ -- this is a keepalive

```bash
$ $sigma 'n1p("a" 1)("b" 2)("a" 3)("a" τ)("b" τ)@-
          K *|p@>@; g'
"a" (1) (3)
"b" (2)
τ
$ $sigma 'n1p("a" 1)("b" 2)("a" ω)("a" 3)("b" 4)@-
          K *|p@>@; g'
"a" (1)
"a" (3)
"b" (2) (4)
τ
```


## Asymmetric cross-multiplexing
The use case here is _mixing:_ for example, we have inputs of the form `user document edit` and we want to multiplex on documents and publish updates to users. Let's define a complete system around this problem.

First, we have multiple full-duplex user connections, each terminating in an edit-state manager before being mixed to documents.

```
(users) ↔ (edit_state)  # ↔ user document edit
```

At this point we need to route edits by document, broadcasting to each involved user. Because we're mapping `user _` to `_ document`, we mix with a connection matrix that implements row/column blocking broadcast/union. This is represented by `'`, which transposes a multiplexing key.

```
(user (edit_state)) ' (document_state) edit_db
```

For example, suppose `edit_state` is identity and we have this series of inputs from users:

```
user_1 doc_1 foo
user_1 doc_2 bar
user_2 doc_3 bif
user_2 doc_1 baz
user_1 doc_1 bok
```

`'` would have the following matrix state after each input:

```
foo [ (u1 d1) ]
bar [ (u1 d1) (u1 d2) ]
bif [ (u1 d1) (u1 d2) 0
      0       0       (u2 d3) ]
baz [ (u1 d1) (u1 d2) 0
      (u2 d1) 0       (u2 d3) ]
bok [ (u1 d1) (u1 d2) 0
      (u2 d1) 0       (u2 d3) ]
```

Let's suppose we're working with the post-`bok` state. There are three types of ηs we might observe:

+ `user document x`: route directly
+ `user τ x →`: broadcast across documents
+ `← document τ x`: broadcast across users

For example, "user 1 disconnected" would be encoded as `u1 τ ω` and would be broadcast to `d1` and `d2`. Similarly, "document 1 was updated" would be `d1 τ x` and would be sent to `u1` and `u2`. More concretely:

```
u1 τ ω            → ' → d1 u1 ω ; d2 u1 ω
u1 d1 x ; u2 d1 x ← ' ← d1 τ x
```

So the purpose of `'` is twofold: first, we swap the first two tuple elements; and second, we allow _τ_ as a broadcast marker to inform all connected parties.

We can disconnect from `'` using _ω_; for example, `u1 d1 ω` will remove the `(u1 d1)` matrix entry. Similarly, `u1 d1 α` will create the matrix entry without sending an edit. Dynamic multiplexers emit _α_ and _ω_ automatically.
