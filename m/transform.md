# Transform code
`ni` relies on Perl for record transformation, but I want τ to implement a custom bytecode that is better optimized for it. Ideally we get all of `ni` functionality with a more concise and targeted notation.

This is a natural fit for a stack language, so we'd use RPN and tight PEG-driven addressing. Or maybe just APL-style notation to avoid stack silliness. I think I prefer APL.


## Backreferences
Let's paint a visual picture for a moment. We have a left-to-right concatenative pipeline just like `ni`, but each operator can have multiple inputs and multiple outputs.

```
op1 op2 op3                   <- linear pipeline

    _____
   /     \
op1--op2--op3                 <- MIMO pipeline
```

UI-wise, we can reach backwards to pick up connections by entering a selection-mode, then the UI will allocate letters/numbers to the most recent _n_ connections. So drawing the line from `op1` to `op3` would be two keystrokes in many cases. (We can also use a more global search.)

**Q:** can we use the same selection machinery with more persistent anchors to create edges during dynamic fabric updates? That is, does the fabric accept UI keystrokes to do the same thing?


## Designing the bytecode
τ is meant for long-running applications, not just one-off data science commands; but I think we should be able to run `ni`-style workflows easily. To that end, let's collect `ni` commands from other places and see how we'd encode them in τ.

```sh
$ ni e[find ~ -xdev -name '*.md'] rp'!/\/ni\//' \
     \<p'ru{/^\S/} if /^\$ ni /' zz\>ni-commands
```


### First principles
+ τ is fundamentally `ni` but with much more powerful topology and state
+ It should be possible to back into existing languages
  + When we do, it should be easy to include custom library code
  + ...implying that operators can consult others' state/conf
+ Like in `ni`, τ records are the fundamental atomic unit of data
+ Operators can react to and manipulate stream annotations like _τ_, _α_, ...

Some things should be trivial:

+ File IO with all multiplex variants (binary data format)
  + Disk-backed cyclic sorting
  + Note that these IOs are `epoll`ed into the same process
+ Common map/filter operations (in-core transform bytecode)
  + Language shellout
+ Multiplex/demultiplex for stream splits
  + Multiplex over CPUs (`ni S`)
  + Data-driven multiplex
+ Generalized FDs-down-streams
  + Websocket and other common network adapters
  + UNIX domain sockets
  + TCP servers (a building block on which we can `ws` adapt, possibly with shorthand)
  + `mmap`ped files/regions? (can create page faults)
  + Operators like `dup()` so we can share FDs when we want to
+ Distributed compute
  + Process boundary splits
  + Machine boundary splits
  + Same-architecture self-deployment? (launch wrapper)


### Dealing with _τ_ markers
Most operators have sensible behavior for _τ_ and other stream markers:

+ _α_, _ω_, _ι_, and _κ_ are single-hop, but multi-hop if multiplexed
+ _τ(n)_ is single-hop, but multi-hop through 1:1 operators
+ _τ(0)_ is multi-hop, but single-hop if the operator collapses _τ_ groups

Because all markers are first-class, operators have a great deal of latitude in how they manipulate them.


### Generalizing `ni`
In general, `ni` excels at one-record manipulation and suffers when dealing with multiple rows at a time (with obvious exceptions like `ru{}`, which are pretty good). For example, `,sgA` is a terrible construct that needs several improvements:

1. The reduction should be customizable, vs having `,sgA` and `,agA`
2. We should be able to reduce multiple columns at once, perhaps in different ways
3. We should be able to reduce the same column different ways (e.g. mean/min/max)

We really want some type of spreadsheet context to apply to groups of rows; in τ we'd create a τ group around the first column and then either do a full snapshot buffer (to run arbitrary computations) or a forward-only column reduction (to run folds).

_τ_ symbol annotations help a lot because we can do bounded locally-streaming computations.
