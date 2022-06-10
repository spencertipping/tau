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


## Collecting some `ni` commands for analysis
τ is meant for long-running applications, not just one-off data science commands; but I think we should be able to run `ni`-style workflows easily. To that end, let's collect `ni` commands from other places and see how we'd encode them in τ.

```sh
$ ni e[find ~ -xdev -name '*.md'] rp'!/\/ni\//' \
     \<p'ru{/^\S/} if /^\$ ni /' zz\>ni-commands
```


## Core bytecode ideas
+ τ is fundamentally `ni` but with much more powerful topology and state
+ It should be possible to back into existing languages
  + When we do, it should be easy to include custom library code
  + ...implying that operators can consult others' state/conf
+ Like in `ni`, τ records are the fundamental atomic unit of data
  + Unlike in `ni`, τ records can hold binary contents and have complex structure
+ Operators can react to and manipulate stream annotations like _τ_, _α_, ...

Things that should be trivial:

+ File IO with all multiplex variants (binary data format)
  + Disk-backed cyclic sorting
  + Note that these IOs are `epoll`ed into the same process
+ Common map/filter operations (in-core transform bytecode)
  + Language shellout with transparent serialization
+ Vertical map
  + Reduce-on-column within _τ_ group (streaming, not in-memory)
  + Store state across input records
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

Note that vertical reduction is just a combination of two simple things: (1) defining a reduction topology, and (2) emit+reset state registers when _τ_ is encountered.

If we look at bytecode as happening within a micro-fabric, then the final "write output to next thing" operator is an explicit instruction for maps and filters. We don't have such an operator for vertical reductions; instead, it emits on _τ_. This is interesting because it means we can use bytecode macros for common stuff like vertical reductions.
